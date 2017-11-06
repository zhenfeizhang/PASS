#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "pass_types.h"
#include "ntt.h"
#include "rng.h"
#include "pass.h"
#include "api.h"

#include "poly.h"
#include "formatc.h"
#include "bsparseconv.h"
#include "hash.h"
#include "fastrandombytes.h"

#define CLEAR(f) memset((f), 0, PASS_N*sizeof(int64))

#define RAND_LEN (4096)

//static uint16 randpool[RAND_LEN];
//static int randpos;

int
crypto_sign_keypair(unsigned char *pk, unsigned char *sk){

	int64 key[PASS_N];
	gen_key(key);

  // printf("\n\nKey: ");
  // for(int i=0; i<PASS_N; i++)
  //   printf("%lld, ", ((long long int) key[i]));

	int64 pubkey[PASS_N] = {0};
	gen_pubkey(pubkey, key);

  // printf("\n\nPubkey: ");
  // for(int i=0; i<PASS_N; i++)
  //   printf("%lld, ", ((long long int) pubkey[i]));
  
  sk = (unsigned char*)key;
  pk = (unsigned char*)pubkey;

  return 0;
}


// sm-->h, smlen-->z, m-->message, mlen-->msglen, sk-->key
int
crypto_sign(unsigned char *sm, unsigned long long *smlen,
            const unsigned char *m, unsigned long long mlen,
            const unsigned char *sk){
  int count;
  b_sparse_poly c;
  int64 y[PASS_N];
  int64 Fy[PASS_N];
  unsigned char msg_digest[HASH_BYTES];

  crypto_hash_sha512(msg_digest, m, mlen);

  //convert
  int64 key[PASS_N];
  for(int i=0; i<PASS_N; i++){
    key[i] = (int64)sk[i];
  }

  count = 0;
  do {
    CLEAR(Fy);

    mknoise(y);
    ntt(Fy, y);
    hash(sm, Fy, msg_digest);

    CLEAR(c.val);
    formatc(&c, sm);

    /* z = y += f*c */
    bsparseconv(y, key, &c);
    /* No modular reduction required. */

    count++;
  } while (reject(y));

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

  memcpy(smlen, y, PASS_N*sizeof(int64));

  return count;
}


// m-->h, mlen-->z
int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk){
  int i;
  b_sparse_poly c;
  int64 Fc[PASS_N] = {0};
  int64 Fz[PASS_N] = {0};
  unsigned char msg_digest[HASH_BYTES];
  unsigned char h2[HASH_BYTES];

  //convert
  int64 z[PASS_N];
  for(int i=0; i<PASS_N; i++){
    z[i] = (int64)mlen[i];
  }

  if(reject(z)) //smlen
    return INVALID;

  CLEAR(c.val);
  formatc(&c, m);

  ntt(Fc, c.val);
  ntt(Fz, z); //smlen

  for(i=0; i<PASS_t; i++) {
    Fz[S[i]] -= Fc[S[i]] * pk[S[i]];
  }

  poly_cmod(Fz);

  crypto_hash_sha512(msg_digest, sm, smlen);
  hash(h2, Fz, msg_digest);

  for(i=0; i<HASH_BYTES; i++) {
    if(h2[i] != m[i])
      return INVALID;
  }

  return VALID;
}


