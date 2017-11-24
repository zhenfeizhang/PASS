/*
 * CPASSREF/sign.c
 *
 *  Copyright 2013 John M. Schanck
 *
 *  This file is part of CPASSREF.
 *
 *  CPASSREF is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CPASSREF is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CPASSREF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "constants.h"
#include "pass_types.h"
#include "poly.h"
#include "formatc.h"
#include "bsparseconv.h"
#include "ntt.h"
#include "hash.h"
#include "fastrandombytes.h"
#include "pass.h"


#define CLEAR(f) memset((f), 0, PASS_N*sizeof(int64))
#define USE_DGS

#define RAND_LEN (4096)

static uint16 randpool[RAND_LEN];
static int randpos;
const uint8 sigma = 215;
const double M = 7.4;

int
init_fast_prng()
{
  fastrandombytes((unsigned char*)randpool, RAND_LEN*sizeof(uint16));
  randpos = 0;

  return 0;
}

#ifdef USE_DGS
void
rng_uint64(uint64 *r)
{
  if(randpos >= RAND_LEN - sizeof(uint64)/sizeof(uint16))
  {
    fastrandombytes((unsigned char*)randpool, RAND_LEN*sizeof(uint16));
    randpos = 0;
  }
  *r = 0;
  *r |= ((uint64)(randpool[randpos++])) << 060;
  *r |= ((uint64)(randpool[randpos++])) << 040;
  *r |= ((uint64)(randpool[randpos++])) << 020;
  *r |= ((uint64)(randpool[randpos++]));

  return;
}

void DGS (      int64   *v,
          const uint16  dim,
          const uint8   stdev)
{
    uint16 d2 = dim/2;
    uint16 i;
    uint64 t;

    static double const Pi=3.141592653589793238462643383279502884L;
    static long const bignum = 0xfffffff;
    double r1, r2, theta, rr;

    for (i=0;i<d2;i++)
    {
        rng_uint64(&t);
        r1 = (1+(t&bignum))/((double)bignum+1);
        r2 = (1+((t>>32)&bignum))/((double)bignum+1);
        theta = 2*Pi*r1;
        rr = sqrt(-2.0*log(r2))*stdev;
        v[2*i] = (int64) floor(rr*sin(theta) + 0.5);
        v[2*i+1] = (int64) floor(rr*cos(theta) + 0.5);
    }

    if (dim%2 == 1)
    {
        rng_uint64(&t);
        r1 = (1+(t&bignum))/((double)bignum+1);
        r2 = (1+((t>>32)&bignum))/((double)bignum+1);
        theta = 2*Pi*r1;
        rr = sqrt(-2.0*log(r2))*stdev;
        v[dim-1] = (int64) floor(rr*sin(theta) + 0.5);
    }
}
#endif

int
mknoise(int64 *y)
{
#ifndef USE_DGS
  int i = 0;
  int x;
  while(i < PASS_N) {
    if(randpos == RAND_LEN) {
      fastrandombytes((unsigned char*)randpool, RAND_LEN*sizeof(uint16));
      randpos = 0;
    }
    x = randpool[randpos++];

    if(x >= UNSAFE_RAND_k) continue;

    x &= (2*PASS_k + 1);

    y[i] = x - PASS_k;
    i++;
  }
#else
  DGS(y,PASS_N,sigma);
#endif

  return 0;
}

/*
 * Square of the Euclidean norm of v
 */
int64 vector_norm2(const int64 *v, uint32 n)
{
  uint32 i;
  int64 sum;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += v[i] * v[i];
  }

  return sum;
}

/*
 * Scalar product of v1 and v2
 */
int64 vector_scalar_product(const int64 *v1, const int64 *v2, uint32 n)
{
  uint32 i;
  int64 sum;

  sum = 0;
  for (i = 0; i < n; i++) {
    sum += v1[i] * v2[i];
  }

  return sum;
}

int
reject(const int64 *z)
{
  int i;

  for(i=0; i<PASS_N; i++) {
    if(abs(z[i]) > (PASS_k - PASS_b))
      return 1;
  }

  return 0;
}

int
sign(unsigned char *h, int64 *z, const int64 *key,
    const unsigned char *message, const int msglen)
{
  int count;
  b_sparse_poly c;
  int64 y[PASS_N];
  int64 Fy[PASS_N];
  unsigned char msg_digest[HASH_BYTES];

  /* For reject sampling */
#ifdef USE_DGS
  int64 fc[PASS_N];
  int64 norm, inner;
  uint64 t;
  double e,p,r;
  static long const bignum = 0xfffffffffffffff;
#endif
  /***********************/

  crypto_hash_sha512(msg_digest, message, msglen);

  count = 0;
  do {
    CLEAR(Fy);

    mknoise(y);
    ntt(Fy, y);
    hash(h, Fy, msg_digest);

    CLEAR(c.val);
    formatc(&c, h);

    /* Compute f*c */
#ifdef USE_DGS
    CLEAR(fc);
    bsparseconv(fc, key, &c);
#endif

    /* z = y += f*c */
    bsparseconv(y, key, &c);
    /* No modular reduction required. */

    count++;

#ifdef USE_DGS
    /*
    for(i = 0; i < PASS_N; i++)
      fprintf(stderr,"%ld ",y[i]);
    fprintf(stderr,"\n");
    */
    norm = vector_norm2(fc,PASS_N);
    // fprintf(stderr,"norm = %ld, ",norm);
    inner = -2*vector_scalar_product(y,fc,PASS_N);
    // fprintf(stderr,"inner = %ld, ",inner);
    e = (double)(norm+inner)/(2*sigma*sigma);
    // fprintf(stderr,"e = %g, ",e);
    p = exp(e)/M;
    // fprintf(stderr,"p = %g, ",p);

    rng_uint64(&t);
    r = (1+(t&bignum))/((double)bignum+1);
    // fprintf(stderr,"r = %g\n",r);
#endif

#ifdef USE_DGS
  } while (reject(y) || r > p);
#else
  }while(reject(y));
#endif

#if DEBUG
  int i;
  printf("\n\ny: ");
  for(i=0; i<PASS_N; i++)
    printf("%lld, ", ((long long int) y[i]));
  printf("\n");

  printf("\n\nFy: ");
  for(i=0; i<PASS_N; i++)
    printf("%lld, ", ((long long int) Fy[i]));
  printf("\n");

  printf("\n\nc: ");
  for(i=0; i<PASS_b; i++)
    printf("(%lld, %lld) ", (long long int) c.ind[i],
        (long long int) c.val[c.ind[i]]);
  printf("\n");
#endif

  memcpy(z, y, PASS_N*sizeof(int64));

  return count;
}

