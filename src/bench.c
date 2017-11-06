/*
 * CPASSREF/bench.c
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
#include <time.h>

#include "constants.h"
#include "pass_types.h"
#include "hash.h"
#include "ntt.h"
#include "pass.h"
#include "api.h"

#ifndef VERIFY
#define VERIFY 1
#endif

#ifndef TRIALS
#define TRIALS 10000
#endif

#define MLEN 256


int
main(int argc, char **argv)
{
  int i;
  int count;

  int64 key[PASS_N];
  int64 *z;
  unsigned char in[MLEN+1] = {0};
  unsigned char h[HASH_BYTES];

  memset(in, '0', MLEN);
  z = malloc(PASS_N * sizeof(int64));
  if(z==NULL) {
    fprintf(stderr,"ERROR: Could not allocate memory.\n");
    exit(EXIT_FAILURE);
  }

  init_fast_prng();

  if(ntt_setup() == -1) {
    fprintf(stderr,
        "ERROR: Could not initialize FFTW. Bad wisdom?\n");
    exit(EXIT_FAILURE);
  }

  printf("Parameters:\n\t N: %d, p: %d, g: %d, k: %d, b: %d, t: %d\n\n",
      PASS_N, PASS_p, PASS_g, PASS_k, PASS_b, PASS_t);

  printf("Generating %d signatures %s\n", TRIALS,
          VERIFY ? "and verifying" : "and not verifying");

  //gen_key(key);
  int64 pubkey[PASS_N];

  crypto_sign_keypair((unsigned char*)pubkey, (unsigned char*)key);
  // convert
  // unsigned char sk[PASS_N] = {0};
  // for(int i=0; i<PASS_N; i++){
  //   sk[i] = (unsigned char)key[i];
  // }

#if DEBUG
  printf("sha512(key): ");
  //crypto_hash_sha512(h, (unsigned char*)key, sizeof(int64)*PASS_N);
  crypto_hash_sha512(h, sk, sizeof(int64)*PASS_N);
  for(i=0; i<HASH_BYTES; i++) {
    printf("%.2x", h[i]);
  }
  printf("\n");
#endif

#if VERIFY
  int nbver = 0;
  
  //int64 pubkey[PASS_N] = {0};
  //gen_pubkey(pubkey, key);
#endif

  clock_t c0,c1;
  c0 = clock();

  //TRIALS为签名数目
  count = 0;
  for(i=0; i<TRIALS; i++) {
   in[(i&0xff)]++; /* Hash a different message each time */
    //一个个sign
   //count += sign(h, z, key, in, MLEN);
   count += crypto_sign(h, (unsigned long long*)z, in, MLEN, (unsigned char*)key);

#if VERIFY
   //verify
   //nbver += (VALID == verify(h, z, pubkey, in, MLEN));
   nbver += (VALID == crypto_sign_open(h, (unsigned long long*)z, in, MLEN, (unsigned char*)pubkey));
#endif
  }
  printf("\n");

  c1 = clock();
  //尝试次数
  printf("Total attempts: %d\n",  count);
#if VERIFY
  printf("Valid signatures: %d/%d\n",  nbver, TRIALS);
#endif
  //
  printf("Attempts/sig: %f\n",  (((float)count)/TRIALS));
  printf("Time/sig: %fs\n", (float) (c1 - c0)/(TRIALS*CLOCKS_PER_SEC));

#if DEBUG
  printf("\n\nKey: ");
  for(i=0; i<PASS_N; i++)
    printf("%lld, ", ((long long int) key[i]));

  #if VERIFY
  printf("\n\nPubkey: ");
  for(i=0; i<PASS_N; i++)
    printf("%lld, ", ((long long int) pubkey[i]));
  printf("\n");
  #endif

  printf("\n\nz: ");
  for(i=0; i<PASS_N; i++)
    printf("%lld, ", ((long long int) z[i]));
  printf("\n");
#endif

  free(z);
  ntt_cleanup();
  return 0;
}

