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

defineTypeNameAndDebug(continuityFunctionObject, 0);

addRemovableToRunTimeSelectionTable
(
    functionObject,
    continuityFunctionObject,
    dictionary
);


// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

extern "C"
{
    // dynamicCode:
    // SHA1 = e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad
    //
    // unique function name that can be checked if the correct library version
    // has been loaded
    void continuity_e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad(bool load)
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

const fvMesh& continuityFunctionObject::mesh() const
{
    return refCast<const fvMesh>(obr_);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

continuityFunctionObject::continuityFunctionObject
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

continuityFunctionObject::~continuityFunctionObject()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool continuityFunctionObject::read(const dictionary& dict)
{
    if (false)
    {
        Info<<"read continuity sha1: e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad\n";
    }

//{{{ begin code
    #line 26 "/home/as/openfoam/OpenFOAM-12/test/multiphaseEuler/interfaceComposition/waterAndIsopropanolEvaporation/system/functions/continuity"
volScalarField* dMass =
        new volScalarField
        (
            IOobject("dMass", mesh().time().name(), mesh()),
            mesh(),
            dimensionedScalar(dimMass/dimVolume, 0)
        );
    volScalarField* dMassGas = new volScalarField("dMass.gas", *dMass);
    volScalarField* mass0Gas = new volScalarField("mass0.gas", *dMass);
    volScalarField* dMassLiq = new volScalarField("dMass.liquid", *dMass);
    volScalarField* mass0Liq = new volScalarField("mass0.liquid", *dMass);
    volScalarField* dEnergy =
        new volScalarField
        (
            IOobject("dEnergy", mesh().time().name(), mesh()),
            mesh(),
            dimensionedScalar(dimEnergy/dimVolume, 0)
        );
    volScalarField* dEnergyLiq = new volScalarField("dEnergy.liquid", *dEnergy);
    volScalarField* energy0Liq = new volScalarField("energy0.liquid", *dEnergy);
    volScalarField* dEnergyGas = new volScalarField("dEnergy.gas", *dEnergy);
    volScalarField* energy0Gas = new volScalarField("energy0.gas", *dEnergy);

    const volScalarField& alphaGas =
        mesh().lookupObject<volScalarField>("alpha.gas");
    const volScalarField& alphaLiq =
        mesh().lookupObject<volScalarField>("alpha.liquid");
    const rhoFluidThermo& thermoGas =
        mesh().lookupObject<rhoFluidThermo>("physicalProperties.gas");
    const rhoFluidThermo& thermoLiq =
        mesh().lookupObject<rhoFluidThermo>("physicalProperties.liquid");

    *mass0Gas = alphaGas*thermoGas.rho();
    *mass0Liq = alphaLiq*thermoLiq.rho();

    // !!! Note that this is only a valid measure of energy
    // conservation because the case is of fixed volume and has no
    // kinetic energy
    *energy0Gas = alphaGas*thermoGas.rho()*ea(thermoGas);
    *energy0Liq = alphaLiq*thermoLiq.rho()*ea(thermoLiq);

    dMass->store();
    dMassGas->store();
    mass0Gas->store();
    dMassLiq->store();
    mass0Liq->store();
    dEnergy->store();
    dEnergyGas->store();
    energy0Gas->store();
    dEnergyLiq->store();
    energy0Liq->store();
//}}} end code

    return true;
}


Foam::wordList continuityFunctionObject::fields() const
{
    if (false)
    {
        Info<<"fields continuity sha1: e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad\n";
    }

    wordList fields;
//{{{ begin code
    
//}}} end code

    return fields;
}


bool continuityFunctionObject::execute()
{
    if (false)
    {
        Info<<"execute continuity sha1: e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad\n";
    }

//{{{ begin code
    #line 79 "/home/as/openfoam/OpenFOAM-12/test/multiphaseEuler/interfaceComposition/waterAndIsopropanolEvaporation/system/functions/continuity"
volScalarField& dMass =
        mesh().lookupObjectRef<volScalarField>("dMass");
    volScalarField& dMassGas =
        mesh().lookupObjectRef<volScalarField>("dMass.gas");
    const volScalarField& mass0Gas =
        mesh().lookupObject<volScalarField>("mass0.gas");
    volScalarField& dMassLiq =
        mesh().lookupObjectRef<volScalarField>("dMass.liquid");
    const volScalarField& mass0Liq =
        mesh().lookupObject<volScalarField>("mass0.liquid");

    volScalarField& dEnergy =
        mesh().lookupObjectRef<volScalarField>("dEnergy");
    volScalarField& dEnergyGas =
        mesh().lookupObjectRef<volScalarField>("dEnergy.gas");
    const volScalarField& energy0Gas =
        mesh().lookupObject<volScalarField>("energy0.gas");
    volScalarField& dEnergyLiq =
        mesh().lookupObjectRef<volScalarField>("dEnergy.liquid");
    const volScalarField& energy0Liq =
        mesh().lookupObject<volScalarField>("energy0.liquid");

    const volScalarField& alphaGas =
        mesh().lookupObject<volScalarField>("alpha.gas");
    const volScalarField& alphaLiq =
        mesh().lookupObject<volScalarField>("alpha.liquid");
    const rhoFluidThermo& thermoGas =
        mesh().lookupObject<rhoFluidThermo>("physicalProperties.gas");
    const rhoFluidThermo& thermoLiq =
        mesh().lookupObject<rhoFluidThermo>("physicalProperties.liquid");

    dMassGas = alphaGas*thermoGas.rho() - mass0Gas;
    dMassLiq = alphaLiq*thermoLiq.rho() - mass0Liq;
    dMass = dMassGas + dMassLiq;

    // !!! Note that this is only a valid measure of energy
    // conservation because the case is of fixed volume and has no
    // kinetic energy
    dEnergyGas = alphaGas*thermoGas.rho()*ea(thermoGas) - energy0Gas;
    dEnergyLiq = alphaLiq*thermoLiq.rho()*ea(thermoLiq) - energy0Liq;
    dEnergy = dEnergyGas + dEnergyLiq;
//}}} end code

    return true;
}


bool continuityFunctionObject::write()
{
    if (false)
    {
        Info<<"write continuity sha1: e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad\n";
    }

//{{{ begin code
    
//}}} end code

    return true;
}


bool continuityFunctionObject::end()
{
    if (false)
    {
        Info<<"end continuity sha1: e7fb2942b8d6d12b701d2bfb66dbf62dbb44d7ad\n";
    }

//{{{ begin code
    
//}}} end code

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //

