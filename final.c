#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdint.h>
#include<time.h>
#include<linux/spi/spidev.h>

#define SPI_PATH "/dev/spidev1.0"

int transfer_byte(int fd, unsigned char* send, unsigned char* receive);
//void timer();
int16_t _16bit_unsigned(int lsb, int msb);
float calculate_mtof(float mtof);
float calculate_float(unsigned char byte_array[4]);
void init_power_state(int fd);
void turn_on_fan(int fd);
void read_hist_loop(int fd);
void turn_off(int fd);

int main()
{
	int fd;
	uint8_t mode = 1; // SPI mode 1
	uint8_t bits = 8;
	uint32_t speed = 400000;
	
	if ((fd = open(SPI_PATH, O_RDWR))<0){
           perror("SPI Error: Can't open device.");
           return -1;
         }
    	if (ioctl(fd, SPI_IOC_WR_MODE, &mode)==-1){
           perror("SPI: Can't set SPI mode.");

           return -1;
         }
    	if (ioctl(fd, SPI_IOC_RD_MODE, &mode)==-1){
           perror("SPI: Can't get SPI mode.");
           return -1;
         }
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)==-1){
	   perror("SPI: Can't set bits per word.");
	   return -1;
	 }
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits)==-1){
	   perror("SPI: Can't get bits per word.");
	   return -1;
	 }
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)==-1){
	   perror("SPI: Can't set max speed HZ");
	   return -1;
	 }
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed)==-1){
	   perror("SPI: Can't get max speed HZ.");
	   return -1;
	 }
	
	init_power_state(fd);
	turn_on_fan(fd);
	read_hist_loop(fd);
	turn_off(fd);

	sleep(1);
	close(fd);
	return 0;
}

int transfer_byte(int fd, unsigned char* send, unsigned char* receive)
{
	struct spi_ioc_transfer transfer = {
	.tx_buf = (unsigned long)(send),
	.rx_buf = (unsigned long)(receive),
	.len = 1,
	.delay_usecs = 2,
	.speed_hz = 400000,
	.bits_per_word = 8,
	.cs_change = 0,	
	};

	int status = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
	if (status < 0) {
	  perror("SPI: SPI_IOC_MESSAGE Failed");
	}
	return 0;
}

/*void timer(){
    time_t timer1;
    char buffer [100];
    time(&timer1);
    sprintf(buffer,"%s",ctime(&timer1));
    printf("%s\n", buffer);
}*/

int16_t _16bit_unsigned(int lsb, int msb)
{
	int16_t value;
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

void init_power_state(int fd)
{
    int j = 0;
    unsigned char data = 0x03, receive;
    printf("Initiate control of power state 1\n");
    transfer_byte(fd, &data, &receive);
    printf("Received byte: %.2X\n", receive);
    while(receive != 0xF3){
        usleep(10000);
        transfer_byte(fd, &data, &receive);
        printf("Received byte: %.2X\n", receive);
        printf("Wait F3\n");
        j = j + 1;
        printf("Wait F3\n");
        if(j == 30){
            printf("Too many error wait 5 s for buffer to reset: %d\n", j);
            sleep(5);
            j = 0;
        }
    }
    usleep(20);
}

void turn_on_fan(int fd)
{
    int j = 0;
    unsigned char data1 = 0x03, data2 = 0x07, receive;
    printf("TURN ON Fan");
    transfer_byte(fd, &data1, &receive);
    sleep(5);
    printf("Sleep for 5 seconds");
    printf("Initiate control of Power State 2");
    transfer_byte(fd, &data1, &receive);
    while(receive != 0xF3){
        usleep(10000);
        transfer_byte(fd, &data1, &receive);
        printf("Received byte: %.2X\n", receive);
        printf("Wait F3\n");
        j = j + 1;
        printf("Wait F3\n");
        if(j == 30){
            printf("Too many error wait 5 s for buffer to reset: %d\n", j);
            sleep(5);
            j = 0;
        }
    }
		usleep(20);
		printf("Turn ON Laser");
		transfer_byte(fd, &data2, &receive);
		sleep(1);
		printf("Sleep for 1s");
}

void read_hist_loop(int fd)
{
    int j = 0, i;
    unsigned char data = 0x30, receive;
    int serial[86] = {};
    while(1)
    {
        printf("\nSend Histogram Command\n");
        transfer_byte(fd, &data, &receive);
        printf("Received byte: %.2X\n", receive);
        while(receive != 0xF3)
        {
            usleep(10000);
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            printf("Wait F3\n");
            j = j + 1;
            printf("Wait F3\n");
            if(j == 30){
                printf("Too many error wait 5 s for buffer to reset: %d\n", j);
                sleep(5);
                j = 0;
            }
        } 
        printf("receive = 0xF3, OPC ready\n");
        usleep(20);

        printf("Read Histogram\n");
        for(i = 0; i < 86; i++)
        {
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            serial[i] = receive;
            usleep(20);
        }
        for(i = 0; i < 86; i++)
        {
            printf("%d ", serial[i]);
        }
	
	time_t timer1;
    	char buffer [100];
    	time(&timer1);
    	sprintf(buffer,"%s",ctime(&timer1));
    
    	FILE * fp;
    	fp = fopen("/home/debian/file.txt", "a");
    	fprintf(fp, "\n%s", buffer);
	for(i = 0; i < 86; i++)
	{
		fprintf(fp, "%d, ", serial[i]);
	}
    	fclose(fp);
	//free(serial);
	printf("wait 10s\n");
	sleep(10);
    }

}

void turn_off(int fd)
{
	int j = 0;
	unsigned char data1 = 0x03, data2 = 0x02, data3 = 0x06, receive;
	transfer_byte(fd, &data1, &receive);
	while(receive != 0xF3){
        usleep(10000);
        transfer_byte(fd, &data1, &receive);
        printf("Received byte: %.2X\n", receive);
        printf("Wait F3\n");
        j = j + 1;
        printf("Wait F3\n");
        if(j == 30){
            printf("Too many error wait 5 s for buffer to reset: %d\n", j);
            sleep(5);
            j = 0;
        }
    }
    usleep(20);
    usleep(20000);
    transfer_byte(fd, &data2, &receive);
    sleep(1);

    transfer_byte(fd, &data1, &receive);
    while(receive != 0xF3){
        usleep(10000);
        transfer_byte(fd, &data1, &receive);
        printf("Received byte: %.2X\n", receive);
        printf("Wait F3\n");
        j = j + 1;
        printf("Wait F3\n");
        if(j == 30){
            printf("Too many error wait 5 s for buffer to reset: %d\n", j);
            sleep(5);
            j = 0;
        }
    }
    usleep(20);
    usleep(20000);

    transfer_byte(fd, &data3, &receive);
    sleep(1);
    printf("Turn Off\n");
    printf("Reset\n");
}
