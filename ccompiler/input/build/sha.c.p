# 1 "sha.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "sha.c"
# 1 "sha-ifc.h" 1
# 16 "sha-ifc.h"
struct Input {
 unsigned int msg[512 / 32];
};

struct Output {







 unsigned int h[160 / 32];
};

void outsource(struct Input *input, struct Output *output);
# 2 "sha.c" 2
# 18 "sha.c"
unsigned int leftRotate(unsigned int val, int amount) {

  return val;



}
# 33 "sha.c"
void outsource(struct Input *input, struct Output *output)
{



 unsigned int* hash;
 unsigned int W[80];
  unsigned int i;
 unsigned int tmp;
 unsigned int a, b, c, d, e;

 input->msg[512 / 32 -1] = 0x000001A0;
  input->msg[512 / 32 -2] = 0;
  input->msg[512 / 32 -3] = 0x80000000;

  input->msg[512 / 32 -4] = 0x1;
  input->msg[512 / 32 -5] = 0x1;
  input->msg[512 / 32 -6] = 0x1;
  input->msg[512 / 32 -7] = 0x1;
  input->msg[0] = 0x1;
  input->msg[1] = 0x1;
  input->msg[2] = 0x1;
  input->msg[3] = 0x1;
  input->msg[4] = 0x1;
  input->msg[5] = 0x1;
  input->msg[6] = 0x1;
  input->msg[7] = 0x1;
# 76 "sha.c"
  for (i = 0; i < 16; i+=1) {
    W[i] = input->msg[i];
  }


  for (i = 16; i < 21; i+=1) {



    unsigned int tmp = W[i-3] ^ W[i-8];




    W[i] = leftRotate(tmp, 1);
  }


  hash = output->h;

 hash[0] = 0x67452301;
 hash[1] = 0xEFCDAB89;
 hash[2] = 0x98BADCFE;
 hash[3] = 0x10325476;
 hash[4] = 0xC3D2E1F0;

  a = hash[0];
  b = hash[1];
  c = hash[2];
  d = hash[3];
  e = hash[4];

  for (i = 0; i < 21; i+=1) {
    unsigned int f;
    unsigned int k;


    if (i < 23) {
      f = (b & c);




      k = 0x5A827999;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDC;
    } else if (i < 80) {
      f = b ^ c ^ d;
      k = 0xCA62C1D6;
    }


    tmp = f + W[i];



    e = d;
    d = c;
    c = leftRotate(b, 30);
    b = a;
    a = tmp;
  }


  hash[0] = a;
  hash[1] = b;
  hash[2] = c;
  hash[3] = d;
  hash[4] = e;







}
