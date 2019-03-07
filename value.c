#include <math.h> 
#include <stdio.h> 
#include <stdlib.h>
  
typedef union { 
  
    float f; 
    struct
    { 
        unsigned int mantissa : 23; 
        unsigned int exponent : 8; 
        unsigned int sign : 1; 
  
    } raw; 
} myfloat; 
unsigned int convertToInt(int* arr, int low, int high) 
{ 
    unsigned f = 0, i; 
    for (i = high; i >= low; i--) { 
        f = f + (arr[i] * (powf(2, high - i))); 
    } 
    return f; 
} 
  
int main() 
{ 
    unsigned int ieee[32] 
        = { 0, 
            0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 
            1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 
            0, 1, 0, 0, 0, 1, 1, 1, 0 }; 
    
    myfloat var; 
    unsigned f = convertToInt(ieee, 9, 31); 
    var.raw.mantissa = f; 
    f = convertToInt(ieee, 1, 8); 
    var.raw.exponent = f; 
    var.raw.sign = ieee[0]; 
  
    printf("The float value of the given"
           " IEEE-754 representation is : \n"); 
    printf("%f", var.f); 
} 
  