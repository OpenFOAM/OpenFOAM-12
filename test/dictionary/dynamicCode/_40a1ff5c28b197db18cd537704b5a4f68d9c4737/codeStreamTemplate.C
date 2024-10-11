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
#line 100 "testCalcNewton/#codeStream"
#include "setWriter.H"
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
    void codeStream_40a1ff5c28b197db18cd537704b5a4f68d9c4737
    (
        Ostream& os,
        const dictionary& dict
    )
    {
//{{{ begin code
        #line 68 "testCalcNewton/#codeStream"
// Lookup the coefficients
        const scalar A = dict.lookupScoped<int32_t>("A", true, false);
        const scalar B = dict.lookupScoped<int32_t>("B", true, false);

        const scalar x0 = dict.lookupScoped<int32_t>("x0", true, false);
        const scalar xn = dict.lookupScoped<int32_t>("xn", true, false);
        const int n = dict.lookupScoped<int32_t>("n", true, false);

        const scalar deltax = (xn - x0)/(n - 1);

        Field<scalar> x(n);
        Field<scalar> fx(n);

        forAll(x, i)
        {
            x[i] = x0 + i*deltax;
            fx[i] = x[i] + B*x[i]/sqrt(1 + sqr(x[i])) - A;
        }

        // Write the x-y data
        setWriter::New("raw")->write
        (
            ".",
            "fx",
            coordSet(true,"x", x),
            "f(x)",
            fx
        );
//}}} end code
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //

