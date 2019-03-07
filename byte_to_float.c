#include<stdio.h>
#include<stdlib.h>

//float byte_float(unsigned char buffer[4]);

int main(void)
{
    /*float x;
    unsigned char arr[] = {178, 64, 204, 123};
    x = byte_float(arr);
    printf("%f\n", x);
    return 0; */

    unsigned char buffer[4];
    float out;
    buffer[0] = 0x40;
    buffer[1] = 0x80;
    buffer[2] = 0x00;
    buffer[3] = 0x00;
    out = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
    printf("%f", out);
    return 0;
}

/*float byte_float(unsigned char buffer[4])
{
    float out;
    //buffer[0] = 0x04;
    //buffer[1] = 0x08;
    //buffer[2] = 0x12;
    //buffer[3] = 0x16;

    out = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
    return out;
} */