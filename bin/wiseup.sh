#!/bin/bash
#
# CPASSREF/wiseup.sh
#
# Copyright 2013 John M. Schanck
#
# This file is part of CPASSREF.
#
# CPASSREF is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# CPASSREF is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CPASSREF.  If not, see <http://www.gnu.org/licenses/>.
#

DEST=../data/$1_wisdom.dat

# Assume input is prime
L=$(($1-1))

echo "Generating FFTW wisdom for transform length $L and storing it in data/$1_wisdom.dat"

if [ -e $DEST ]
then
    read -p "$DEST exists. Overwrite? " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]
    then
        fftw-wisdom --exhaustive rof$L rob$L > $DEST
    fi
else
    fftw-wisdom --exhaustive rof$L rob$L > $DEST
fi

