/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) YEAR OpenFOAM Foundation
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

#include "codedFunctionObjectTemplate.H"
#include "volFields.H"
#include "read.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject, 0);

addRemovableToRunTimeSelectionTable
(
    functionObject,
    populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject,
    dictionary
);


// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

extern "C"
{
    // dynamicCode:
    // SHA1 = 12c65017761110a27f53232586fc8ac3bc5d226d
    //
    // unique function name that can be checked if the correct library version
    // has been loaded
    void populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1__12c65017761110a27f53232586fc8ac3bc5d226d(bool load)
    {
        if (load)
        {
            // code that can be explicitly executed after loading
        }
        else
        {
            // code that can be explicitly executed before unloading
        }
    }
}


// * * * * * * * * * * * * * * * Local Functions * * * * * * * * * * * * * * //

//{{{ begin localCode

//}}} end localCode


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

const fvMesh& populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::mesh() const
{
    return refCast<const fvMesh>(obr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    functionObjects::regionFunctionObject(name, runTime, dict)
{
    read(dict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::~populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::read(const dictionary& dict)
{
    if (false)
    {
        Info<<"read populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_ sha1: 12c65017761110a27f53232586fc8ac3bc5d226d\n";
    }

//{{{ begin code
    #line 14 "/home/as/openfoam/OpenFOAM-12/test/multiphaseEuler/populationBalance/coalescence/system/functions/populationBalanceInitialDistributionFs"
const word phaseName = dict.lookup<word>("phase");

    const fileName initialDistributionFileName =
        dict.lookup<fileName>("initialDistributionFile");

    const List<dictionary> sizeGroupDicts
    (
        IOdictionary
        (
            IOobject
            (
                "phaseProperties",
                mesh().time().constant(),
                mesh(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            )
        )
       .subDict(phaseName)
       .subDict("velocityGroupCoeffs")
       .lookup("sizeGroups")
    );

    const Function1s::Table<scalar> initialDistribution
    (
        "initialDistribution",
        dimLength,
        dimless,
        dictionary
        (
            "format", "foam",
            "file", initialDistributionFileName
        )
    );

    const volScalarField fDefault
    (
        IOobject
        (
            IOobject::groupName("fDefault", phaseName),
            mesh().time().name(),
            mesh(),
            IOobject::MUST_READ
        ),
        mesh()
    );

    forAll(sizeGroupDicts, i)
    {
        volScalarField
        (
            IOobject
            (
                IOobject::groupName("f" + Foam::name(i), phaseName),
                mesh().time().name(),
                mesh(),
                IOobject::READ_IF_PRESENT
            ),
            mesh(),
            dimensionedScalar
            (
                dimless,
                initialDistribution.value
                (
                    dimensionedScalar
                    (
                        "dSph",
                        dimLength,
                        sizeGroupDicts[i]
                    ).value()
                )
            ),
            fDefault.boundaryField().types()
        ).write();
    }
//}}} end code

    return true;
}


Foam::wordList populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::fields() const
{
    if (false)
    {
        Info<<"fields populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_ sha1: 12c65017761110a27f53232586fc8ac3bc5d226d\n";
    }

    wordList fields;
//{{{ begin code
    
//}}} end code

    return fields;
}


bool populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::execute()
{
    if (false)
    {
        Info<<"execute populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_ sha1: 12c65017761110a27f53232586fc8ac3bc5d226d\n";
    }

//{{{ begin code
    
//}}} end code

    return true;
}


bool populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::write()
{
    if (false)
    {
        Info<<"write populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_ sha1: 12c65017761110a27f53232586fc8ac3bc5d226d\n";
    }

//{{{ begin code
    
//}}} end code

    return true;
}


bool populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_FunctionObject::end()
{
    if (false)
    {
        Info<<"end populationBalanceInitialDistributionFs_phase_air1_initialDistributionFile_constantinitialDistribution_air1_ sha1: 12c65017761110a27f53232586fc8ac3bc5d226d\n";
    }

//{{{ begin code
    
//}}} end code

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //

