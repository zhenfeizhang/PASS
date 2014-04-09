#!/usr/bin/env python
#
# CPASSREF/cpassref_rader.py
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

import sys
import random


def usage():
    print "python cpassref_rader.py N p g t"

primes1k = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61,\
           67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137,\
          139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211,\
          223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283,\
          293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,\
          383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461,\
          463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563,\
          569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643,\
          647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739,\
          743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829,\
          839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937,\
          941, 947, 953, 967, 971, 977, 983, 991, 997]

def znprimroot(p): # For small p (< 1M)
    global primes1k
    i = 0;
    a = 2;
    phi = p-1;
    P = [z for z in primes1k if (phi%z == 0)]
    bs = [phi/z for z in P]
    while i < len(P):
        if pow(a, bs[i], p) == 1:
            a += 1
            i = 0
            continue
        else:
            i+=1
    return a


def cmod(v, p):
    mx = (p-1)/2;
    for i in range(len(v)):
        if(v[i] > mx):
            v[i] -= p;
    return v

if __name__ == "__main__":
    if len(sys.argv) < 4:
        usage()
        exit()

    N = int(sys.argv[1])
    p = int(sys.argv[2])
    gp = int(sys.argv[3])
    t = int(sys.argv[4])
    gN = znprimroot(N)

    perm = [pow(gN, i, N) for i in [0] + range(N-2, 0, -1)]
    poly = cmod([pow(gp, i, p) for i in [perm[0]] + perm[-1:0:-1]], p)
    points = random.sample(range(1, ((N-1)/2) + 1), (t/2))
    points += [N - i for i in points]
    points.sort()

    permf = "./data/" + str(N) + "_perm.dat"
    polyf = "./data/" + str(N) + "_rader.dat"
    pointsf = "./data/" + str(N) + "_points.dat"

    p1 = raw_input("Will write permutation to "+permf+". OK? [y/n] ")
    if p1 == 'y':
        try:
            open(permf, "w").write(",".join(str(x) for x in perm))
        except e:
            print "Error!", e
    elif p1 == 'n':
        p2 = raw_input("Print permutation? [y/n] ")
        if p2 == 'y':
            print ",".join(str(x) for x in perm) + "\n\n"

    p1 = raw_input("Will write NTT polynomial to "+polyf+". OK? [y/n] ")
    if p1 == 'y':
        try:
            open(polyf, "w").write(",".join(str(x) for x in poly))
        except e:
            print "Error!", e
            pass
    elif p1 == 'n':
        p2 = raw_input("Print polynomial? [y/n] ")
        if p2 == 'y':
            print ",".join(str(x) for x in poly) + "\n\n"

    p1 = raw_input("Will write evaluation set to "+pointsf+". OK? [y/n] ")
    if p1 == 'y':
        try:
            open(pointsf, "w").write(",".join(str(x) for x in points))
        except e:
            print "Error!", e
            pass
    elif p1 == 'n':
        p2 = raw_input("Print evaluation set? [y/n] ")
        if p2 == 'y':
            print ",".join(str(x) for x in points) + "\n\n"
