#include <stdio.h>
#include <math.h>

int main(void) {
  union {
    float f;
    unsigned char uc[sizeof(float)];
  } x;

  // float to bytes
  //x.f = 1.23f;
  //printf("%x %x %x %x\n", x.uc[0], x.uc[1], x.uc[2], x.uc[3]);

  // bytes to float
  x.uc[0] = 142;
  x.uc[1] = 220;
  x.uc[2] = 142;
  x.uc[3] = 63;
  printf("%.8e\n", x.f);
}

float _16bit_unsigned_to_float(unsigned char lsb, unsigned char msb)
{
	union {
		float f;
		unsigned char l, m;
	}out;
	out.l = lsb;
	out.m = msb;
	return out.f;
}