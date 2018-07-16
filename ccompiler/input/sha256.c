#include "sha256-ifc.h"

#ifdef QSP_NATIVE
#include <stdio.h>
#include <string.h>
#endif

// Assumes the message is exactly 416 bits long, 
// so the value we hash is: msg || 1000...00 || len,
// where len = 416 and there are 31 zeros
// This saves us from dealing with padding for now

// Based in part on pseudocode from http://en.wikipedia.org/wiki/SHA256-1
// the spec at http://tools.ietf.org/html/rfc3174,
// and the FIPS 180-2 spec: 
// http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf

// u32 leftRotate(u32 val, int amount) {
//   return (val << amount) | (val >> (32 - amount));
// }

u32 rightRotate(u32 val, int amount) {
  return (val >> amount) | (val << (32 - amount));
}

u32 rightShift(u32 val, int amount) {
  return (val >> amount);
}

// #if PARAM==0
// // Succeeds at 20.  Failed at 21. Failed at 30 and 40
// #define NUM_ROUNDS 21
// #else
#define NUM_ROUNDS 64
// #endif

void outsource(struct Input *input, struct Output *output)
{
	u32* hash;
	u32 W[NUM_ROUNDS];  // Expanded message
  u32 i;
	u32 tmp;
	u32 a, b, c, d, e, f, g, h;

	input->msg[INPUT_SIZE-1] = 0x000001A0;
  input->msg[INPUT_SIZE-2] = 0;     // (length is 64 bits)
  input->msg[INPUT_SIZE-3] = 0x80000000; // Fix the padding

  for (i = 0; i < 16; i+=1) {
    W[i] = input->msg[i];
  }

  // Expand the message further
  for (i = 16; i < NUM_ROUNDS; i+=1) {

    // u32 tmp = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16];

    // W[i] = leftRotate(tmp, 1);

    u32 s0 = rightRotate(W[i-15], 7) ^ rightRotate(W[i-15], 18) ^ rightShift(W[i-15], 3);
    u32 s1 = rightRotate(W[i-2], 17) ^ rightRotate(W[i-2],  19) ^ rightShift(W[i-2], 10);

    W[i] = W[i-16] + s0 + W[i-7] + s1;
  }

  // Initialize variables
  // (first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
  hash = output->h;
	
	hash[0] = 0x6a09e667;
	hash[1] = 0xbb67ae85;
	hash[2] = 0x3c6ef372;
	hash[3] = 0xa54ff53a;
	hash[4] = 0x510e527f;
	hash[5] = 0x9b05688c;
	hash[6] = 0x1f83d9ab;
	hash[7] = 0x5be0cd19;

  // Initialize table of round constants
  // (first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311):
  u32 k[64] =
  {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  a = hash[0];
  b = hash[1];
  c = hash[2];
  d = hash[3];
  e = hash[4];
  f = hash[5];
  g = hash[6];
  h = hash[7];

  for (i = 0; i < NUM_ROUNDS; i+=1) {

    u32 s0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);

    u32 maj = (a & b) ^ (a & c) ^ (b & c);

    u32 t2 = s0 + maj;

    u32 s1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);

    u32 ch = (e & f) ^ ((~e) & g);

    u32 t1 = h + s1 + ch + k[i] + W[i];

    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  hash[0] = hash[0] + a;
  hash[1] = hash[1] + b;
  hash[2] = hash[2] + c;
  hash[3] = hash[3] + d;
  hash[4] = hash[4] + e;
  hash[5] = hash[5] + f;
  hash[6] = hash[6] + g;
  hash[7] = hash[7] + h;
}


#ifdef _WIN32
#include <assert.h>
#include <stdio.h>
#include <string.h>

void main () {
	struct Input  input;
	struct Output output;
	int i, iter;
	char *sha256_in_fn = "win-input.bin";
	FILE* file;

	for (iter = 4; iter < 10; iter++) {
		for (i=0; i<INPUT_SIZE-3; i++)
		{
			input.msg[i] = (i+iter)<<(iter*3);
		}
		for (i=INPUT_SIZE-3; i<INPUT_SIZE; i++)
		{
			input.msg[i] = 0;
		}

		// Use the string "abc"
		input.msg[0] = 0x61626300;


		// Write out a file we can test with sha2561sum
		file = fopen(sha256_in_fn, "wb");
		//fwrite("abc", sizeof(char), 3, file);
		for (i = 0; i < INPUT_SIZE - 3; i++)
		{
			unsigned char* charArr = (unsigned char*)&input.msg[i];
			unsigned int reverse = charArr[3] <<  0 | charArr[2] <<  8
				| charArr[1] << 16 | charArr[0] << 24;
			//printf("%x -> %x, %x %x %x %x\n", input->msg[i], reverse, charArr[3], charArr[2], charArr[1], charArr[0]);
			fwrite(&reverse, sizeof(unsigned int), 1, file);
		}
		fclose(file);


		outsource(&input, &output);

		for (i=0; i<OUTPUT_SIZE; i++)
		{
			printf("%08x", output.h[i]);
		}
		printf("\n");
	}

	getc(stdin);
}
#endif
