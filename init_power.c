#include <stdio.h>
#include <stdlib.h>

void init_power_state()
{
    int fd;
    unsigned char data = 0x03;
    printf("Initiate control of power state 1");
    transfer_byte(fd, &data);
    while()
}