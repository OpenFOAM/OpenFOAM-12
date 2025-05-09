/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2024 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "mappedPatchBase.H"
#include "SubField.H"
#include "Time.H"
#include "triPointRef.H"
#include "treeDataPoint.H"
#include "indexedOctree.H"
#include "globalIndex.H"
#include "RemoteData.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(mappedPatchBase, 0);

    const scalar mappedPatchBase::defaultMatchTol_ = 1e-4;
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::tmp<Foam::vectorField> Foam::mappedPatchBase::patchFaceAreas() const
{
    return patch_.primitivePatch::faceAreas();
}


Foam::tmp<Foam::pointField> Foam::mappedPatchBase::patchFaceCentres() const
{
    return patch_.primitivePatch::faceCentres();
}


Foam::tmp<Foam::pointField> Foam::mappedPatchBase::patchLocalPoints() const
{
    return patch_.localPoints();
}


Foam::tmp<Foam::vectorField> Foam::mappedPatchBase::nbrPatchFaceAreas() const
{
    return
        nbrPatchIsMapped()
      ? nbrMappedPatch().patchFaceAreas()
      : tmp<vectorField>(nbrPolyPatch().primitivePatch::faceAreas());
}


Foam::tmp<Foam::pointField> Foam::mappedPatchBase::nbrPatchFaceCentres() const
{
    return
        nbrPatchIsMapped()
      ? nbrMappedPatch().patchFaceCentres()
      : tmp<vectorField>(nbrPolyPatch().primitivePatch::faceCentres());
}


Foam::tmp<Foam::pointField> Foam::mappedPatchBase::nbrPatchLocalPoints() const
{
    return
        nbrPatchIsMapped()
      ? nbrMappedPatch().patchLocalPoints()
      : tmp<vectorField>(nbrPolyPatch().localPoints());
}


void Foam::mappedPatchBase::calcMapping() const
{
    if (treeMapPtr_.valid())
    {
        FatalErrorInFunction
            << "Mapping already calculated" << exit(FatalError);
    }

    if (sameUntransformedPatch())
    {
        FatalErrorInFunction
            << "Patch " << patch_.name() << " is mapping itself with no "
            << "transform. Mapping data does not need to be constructed."
            << exit(FatalError);
    }

    // Calculate the transform as necessary
    transform_ =
        cyclicTransform
        (
            patch_.name(),
            patchFaceCentres(),
            patchFaceAreas(),
            transform_,
            nbrPolyPatch().name(),
            nbrPatchFaceCentres(),
            nbrPatchFaceAreas(),
            nbrPatchIsMapped()
          ? nbrMappedPatch().transform_
          : cyclicTransform(false),
            matchTol_,
            true
        );

    // Build the mapping
    if (usingTree_)
    {
        const globalIndex patchGlobalIndex(patch_.size());

        // Find processor and cell/face indices of samples
        labelList sampleGlobalPatchFaces, sampleIndices;
        {
            // Gather the sample points into a single globally indexed list
            List<point> allPoints(patchGlobalIndex.size());
            {
                List<pointField> procSamplePoints(Pstream::nProcs());
                procSamplePoints[Pstream::myProcNo()] =
                    transform_.transform().invTransformPosition
                    (
                        patchFaceCentres()
                    );
                Pstream::gatherList(procSamplePoints);
                Pstream::scatterList(procSamplePoints);

                forAll(procSamplePoints, proci)
                {
                    forAll(procSamplePoints[proci], procSamplei)
                    {
                        allPoints
                        [
                            patchGlobalIndex.toGlobal(proci, procSamplei)
                        ] = procSamplePoints[proci][procSamplei];
                    }
                }
            }

            // List of possibly remote mapped faces
            List<RemoteData<scalar>> allNearest(patchGlobalIndex.size());

            // Find nearest face opposite every face
            if (nbrPolyPatch().empty())
            {
                forAll(allPoints, alli)
                {
                    allNearest[alli].proci = -1;
                    allNearest[alli].elementi = -1;
                    allNearest[alli].data = Foam::sqr(great);
                }
            }
            else if (nbrPolyPatch().size() == 1)
            {
                const pointField nbrPoints(nbrPatchFaceCentres());

                forAll(allPoints, alli)
                {
                    const point& p = allPoints[alli];

                    allNearest[alli].proci = Pstream::myProcNo();
                    allNearest[alli].elementi = 0;
                    allNearest[alli].data = magSqr(nbrPoints[0] - p);
                }
            }
            else
            {
                const pointField nbrPoints(nbrPatchFaceCentres());

                const treeBoundBox nbrPointsBb
                (
                    treeBoundBox(nbrPoints).extend(1e-4)
                );

                const indexedOctree<treeDataPoint> tree
                (
                    treeDataPoint(nbrPoints),
                    nbrPointsBb,    // overall search domain
                    8,              // maxLevel
                    10,             // leafsize
                    3.0             // duplicity
                );

                forAll(allPoints, alli)
                {
                    const point& p = allPoints[alli];

                    const pointIndexHit pih =
                        tree.findNearest(p, magSqr(nbrPointsBb.span()));

                    if (pih.hit())
                    {
                        allNearest[alli].proci = Pstream::myProcNo();
                        allNearest[alli].elementi = pih.index();
                        allNearest[alli].data =
                            magSqr(nbrPoints[pih.index()] - p);
                    }
                }
            }

            // Find nearest. Combine on master.
            Pstream::listCombineGather
            (
                allNearest,
                RemoteData<scalar>::smallestEqOp()
            );
            Pstream::listCombineScatter(allNearest);

            // Determine the number of faces for which a nearest neighbouring
            // face was not found (no reduction necessary as this is computed
            // from synchronised data)
            label nNotFound = 0;
            forAll(allPoints, alli)
            {
                if (allNearest[alli].proci == -1)
                {
                    nNotFound ++;
                }
            }

            // If any points were not found within cells then re-search for
            // them using a nearest test, which should not fail. Warn that this
            // is happening. If any points were not found for some other
            // method, then fail.
            if (nNotFound)
            {
                FatalErrorInFunction
                    << "Mapping failed for " << nl
                    << "    patch: " << patch_.name() << nl
                    << "    neighbourRegion: " << nbrRegionName() << nl
                    << "    neighbourPatch: " << nbrPatchName() << nl
                    << exit(FatalError);
            }

            // Build lists of samples
            DynamicList<label> samplePatchGlobalFacesDyn;
            DynamicList<label> sampleIndicesDyn;
            forAll(allNearest, alli)
            {
                if (allNearest[alli].proci == Pstream::myProcNo())
                {
                    samplePatchGlobalFacesDyn.append(alli);
                    sampleIndicesDyn.append(allNearest[alli].elementi);
                }
            }
            sampleGlobalPatchFaces.transfer(samplePatchGlobalFacesDyn);
            sampleIndices.transfer(sampleIndicesDyn);
        }

        // Construct distribution schedule
        List<Map<label>> compactMap;
        treeMapPtr_.reset
        (
            new distributionMap
            (
                patchGlobalIndex,
                sampleGlobalPatchFaces,
                compactMap
            )
        );
        const labelList oldToNew(move(sampleGlobalPatchFaces));
        const labelList oldSampleIndices(move(sampleIndices));

        // Construct input mapping for data to be distributed
        treeNbrPatchFaceIndices_ = labelList(treeMapPtr_->constructSize(), -1);
        UIndirectList<label>(treeNbrPatchFaceIndices_, oldToNew) =
            oldSampleIndices;

        // Reverse the map. This means the map is "forward" when going from
        // the neighbour patch to this patch, which is logical.
        treeMapPtr_.reset
        (
            new distributionMap
            (
                patch_.size(),
                move(treeMapPtr_->constructMap()),
                move(treeMapPtr_->subMap())
            )
        );
    }
    else
    {
        if (patchToPatchIsValid_)
        {
            FatalErrorInFunction
                << "Patch-to-patch already calculated" << exit(FatalError);
        }

        const pointField patchLocalPoints(this->patchLocalPoints());
        const pointField nbrPatchLocalPoints(this->nbrPatchLocalPoints());

        const primitivePatch patch
        (
            SubList<face>(patch_.localFaces(), patch_.size()),
            patchLocalPoints
        );
        const primitivePatch nbrPatch
        (
            SubList<face>(nbrPolyPatch().localFaces(), nbrPolyPatch().size()),
            nbrPatchLocalPoints
        );

        patchToPatchPtr_->update
        (
            patch,
            patch.pointNormals(),
            nbrPatch,
            transform_.transform()
        );

        patchToPatchIsValid_ = true;
    }
}


// * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * * * * //

Foam::mappedPatchBase::mappedPatchBase(const polyPatch& pp)
:
    mappedPatchBaseBase(pp),
    usingTree_(true),
    treeMapPtr_(nullptr),
    treeNbrPatchFaceIndices_(),
    patchToPatchIsValid_(false),
    patchToPatchPtr_(nullptr),
    matchTol_(defaultMatchTol_),
    reMapAfterMove_(true),
    reMapNbr_(false)
{}


Foam::mappedPatchBase::mappedPatchBase
(
    const polyPatch& pp,
    const word& nbrRegionName,
    const word& nbrPatchName,
    const cyclicTransform& transform
)
:
    mappedPatchBaseBase(pp, nbrRegionName, nbrPatchName, transform),
    usingTree_(true),
    treeMapPtr_(nullptr),
    treeNbrPatchFaceIndices_(),
    patchToPatchIsValid_(false),
    patchToPatchPtr_(nullptr),
    matchTol_(defaultMatchTol_),
    reMapAfterMove_(true),
    reMapNbr_(false)
{}


Foam::mappedPatchBase::mappedPatchBase
(
    const polyPatch& pp,
    const dictionary& dict,
    const bool defaultTransformIsNone
)
:
    mappedPatchBaseBase(pp, dict, defaultTransformIsNone),
    usingTree_(!dict.found("method") && !dict.found("sampleMode")),
    treeMapPtr_(nullptr),
    treeNbrPatchFaceIndices_(),
    patchToPatchIsValid_(false),
    patchToPatchPtr_
    (
        !usingTree_
      ? patchToPatch::New
        (
            dict.lookupBackwardsCompatible<word>({"method", "sampleMode"}),
            false
        ).ptr()
      : nullptr
    ),
    matchTol_(dict.lookupOrDefault("matchTolerance", defaultMatchTol_)),
    reMapAfterMove_(dict.lookupOrDefault<bool>("reMapAfterMove", true)),
    reMapNbr_(false)
{}


Foam::mappedPatchBase::mappedPatchBase
(
    const polyPatch& pp,
    const mappedPatchBase& mpb
)
:
    mappedPatchBaseBase(pp, mpb),
    usingTree_(mpb.usingTree_),
    treeMapPtr_(nullptr),
    treeNbrPatchFaceIndices_(),
    patchToPatchIsValid_(false),
    patchToPatchPtr_
    (
        !usingTree_
      ? patchToPatch::New(mpb.patchToPatchPtr_->type(), false).ptr()
      : nullptr
    ),
    matchTol_(mpb.matchTol_),
    reMapAfterMove_(mpb.reMapAfterMove_),
    reMapNbr_(false)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::mappedPatchBase::~mappedPatchBase()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::mappedPatchBase& Foam::mappedPatchBase::getMap
(
    const polyPatch& patch
)
{
    if (!isA<mappedPatchBase>(patch))
    {
        FatalErrorInFunction
            << "Patch " << patch.name() << " is not of type "
            << typeName << exit(FatalError);
    }

    return refCast<const mappedPatchBase>(patch);
}


void Foam::mappedPatchBase::clearOut()
{
    if (reMapAfterMove_)
    {
        treeMapPtr_.clear();
        treeNbrPatchFaceIndices_.clear();
        patchToPatchIsValid_ = false;
        reMapNbr_ = true;
    }
}


void Foam::mappedPatchBase::write(Ostream& os) const
{
    mappedPatchBaseBase::write(os);

    if (!usingTree_)
    {
        writeEntry(os, "method", patchToPatchPtr_->type());
    }

    writeEntryIfDifferent(os, "matchTolerance", defaultMatchTol_, matchTol_);

    writeEntryIfDifferent<bool>(os, "reMapAfterMove", true, reMapAfterMove_);
}


// ************************************************************************* //
