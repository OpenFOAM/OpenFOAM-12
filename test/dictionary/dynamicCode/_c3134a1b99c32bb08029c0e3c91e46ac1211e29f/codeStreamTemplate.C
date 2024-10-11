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

Description
    Template for use with codeStream.

\*---------------------------------------------------------------------------*/

#include "dictionary.H"
#include "fieldTypes.H"
#include "Ostream.H"
#include "Pstream.H"
#include "read.H"
#include "unitConversion.H"

//{{{ begin codeInclude

//}}} end codeInclude

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * Local Functions * * * * * * * * * * * * * * //

//{{{ begin localCode

//}}} end localCode


// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

extern "C"
{
    void codeStream_c3134a1b99c32bb08029c0e3c91e46ac1211e29f
    (
        Ostream& os,
        const dictionary& dict
    )
    {
//{{{ begin code
        #line 1 ""
// Lookup the coefficients
    const scalar A = dict.lookupScoped<int32_t>("A", true, false);
    const scalar B = dict.lookupScoped<int32_t>("B", true, false);

    // Initial guess for x
    scalar x = 0;

    scalar x0 = x;

    do
    {
        // Store the previous iteration x for the convergence check
        x0 = x;

        // Temporary sub-function evaluations
        const scalar f1 = 1 + sqr(x);
        const scalar f2 = sqrt(f1);

        // Evaluate the function
        const scalar f = x + B*x/f2 - A;

        // Evaluate the derivative
        const scalar df = 1 + B/(f1*f2);

        // Update x
        x = x0 - f/df;

        // Test for convergence
    } while (mag(x - x0) > 1e-6);

    os << x;
//}}} end code
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //

