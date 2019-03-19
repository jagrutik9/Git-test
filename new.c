#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdint.h>
#include<linux/spi/spidev.h>

#define SPI_PATH "/dev/spidev1.0"

int transfer_byte(int fd, unsigned char* send, unsigned char* receive){

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
	return status;
}





int main()
{
	int fd;
	uint8_t mode = 1; // SPI mode 1
	uint8_t bits = 8;
	uint32_t speed = 400000;
	unsigned char data = 0x03, data1 = 0x30;
	unsigned char receive;
	
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
	
	transfer_byte(fd, &data, &receive);
	printf("1st received byte: %.2X\n", receive);	
	transfer_byte(fd, &data1, &receive);
	printf("2nd received byte: %.2X\n", receive);
	sleep(1);
	close(fd);
	return 0;
}
