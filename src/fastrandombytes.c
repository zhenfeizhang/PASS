/*
 * File:   lattisigns512-20130329/fastrandombytes.c
 * Author: Gim Güneysu, Tobias Oder, Thomas Pöppelmann, Peter Schwabe
 * Public Domain
 */

#include "crypto_stream_salsa20.h"
#include "rng.h"

static int init = 0;
static unsigned char key[crypto_stream_salsa20_KEYBYTES];
static unsigned char nonce[crypto_stream_salsa20_NONCEBYTES] = {0};

void fastrandombytes(unsigned char *r, unsigned long long rlen)
{
  unsigned long long n=0;
  int i;

  // This is the test input string
  unsigned char entropy_input[48] = {0};
  for (int i=0; i<48; i++){
    entropy_input[i] = 't';
  } 
  unsigned char personalization_string[48] = {0};
  for (int i=0; i<48; i++){
    personalization_string[i] = 'z';
  }
  int security_strength = 48;
  randombytes_init(entropy_input, personalization_string, security_strength);
  // test end
  
  if(!init)
  {
    randombytes(key, crypto_stream_salsa20_KEYBYTES);
    init = 1;
  }
  crypto_stream(r,rlen,nonce,key);
  //crypto_stream_salsa20(r,rlen,nonce,key);

  // Increase 64-bit counter (nonce)
  for(i=0;i<8;i++)
    n ^= ((unsigned long long)nonce[i]) << 8*i;
  n++;
  for(i=0;i<8;i++)
    nonce[i] = (n >> 8*i) & 0xff;
}
