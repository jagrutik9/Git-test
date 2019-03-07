#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

__int16_t _16bit_unsigned(int lsb, int msb);
float calculate_mtof(float mtof);
float calculate_float(unsigned char byte_array[4]);


int main(void)
{
	int lsb = 12, msb = 2;
    int serial = 79;
	__int16_t data1;
	float data2, x;
    unsigned char arr[] = {142, 220, 142, 63};
    x = calculate_float(arr);
	data1 = _16bit_unsigned(lsb, msb);
	data2 = calculate_mtof(serial);
	printf("Converted 16_bit unsigned Value: %d\n", data1);
	printf("MTOF Value: %f\n", data2);
	printf("Array to float Value: %20.18f\n", x);
	return 0;
}

__int16_t _16bit_unsigned(int lsb, int msb)
{
	__int16_t value;
	value = (msb << 8) + lsb;
	return value;
}

float calculate_mtof(float mtof)
{
	float value = mtof / (float)3.0;
	return value;
}

float calculate_float(unsigned char byte_array[4])
{
	union {
		float f;
		unsigned char arr[4];
	}out;
	out.arr[0] = byte_array[0];
	out.arr[1] = byte_array[1];
	out.arr[2] = byte_array[2];
	out.arr[3] = byte_array[3];
	return out.f;
}