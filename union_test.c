#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main()
{
    union test
{
   unsigned char buf[4];
   float number;
}test;
test.buf[0] = 0x00;
test.buf[1] = 0x00;
test.buf[2] = 0x80;
test.buf[3] = 0x40;
test.number = (test.buf[3] << 24) | (test.buf[2] << 16) | (test.buf[1] << 8) | test.buf[0];

printf("%f\n",test.number);
return 0;

}