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
void read_info_string(int fd);
void set_fan_digital_pot(int fd);
void read_dac_power_status(int fd);
void set_bin_index(int fd);
void read_config_variable(int fd);

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
	
	read_info_string(fd);
	set_fan_digital_pot(fd);
	read_dac_power_status(fd);
	set_bin_index(fd);
	read_config_variable(fd);
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
    printf("TURN ON Fan\n");
    transfer_byte(fd, &data1, &receive);
    sleep(5);
    printf("Sleep for 5 seconds\n");
    printf("Initiate control of Power State 2\n");
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
		printf("Turn ON Laser\n");
		transfer_byte(fd, &data2, &receive);
		sleep(1);
		printf("Sleep for 1s\n");
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
    	fp = fopen("/home/debian/opc_data.txt", "a");
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

void read_info_string(int fd)
{
    int j = 0, i;
    unsigned char infostring[60] = {};
    unsigned char data1 = 0x00, data2 = 0x3F, receive;
    transfer_byte(fd, &data1, &receive);
    sleep(2);
    transfer_byte(fd, &data2, &receive);
    while(receive != 0xF3)
        {
            usleep(10000);
            transfer_byte(fd, &data2, &receive);
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
    usleep(100000);
    for(i = 0; i < 60; i++)
    {
        transfer_byte(fd, &data2, &receive);
        printf("Infostring: %.2X\n", receive);
        infostring[i] = receive;
        if(i == 59)
        {
            printf("Received byte: %.2X\n", receive);
            //i = 0;
        }
        usleep(10000);
    }
    for(i = 0; i < 60; i++)
    {
        printf("%c ", infostring[i]);
    }    
}

void set_fan_digital_pot(int fd)
{
    int j = 0;
    unsigned char data1 = 0x42, data2 = 0x01, data3 = 168, receive;
    transfer_byte(fd, &data1, &receive);
    usleep(10000);
    while(receive != 0xF3)
        {
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
    usleep(10000);
    printf("Set fan or laser digital pot\n");
    transfer_byte(fd, &data2, &receive);
    printf("0x42: %u\n", receive);
    transfer_byte(fd, &data3, &receive);
    printf("Channel 1?: %u\n", receive);
    usleep(100000);
}

void read_dac_power_status(int fd)
{
    int j = 0;
    unsigned char data = 0x13, receive;
    transfer_byte(fd, &data, &receive);
    usleep(10000);
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
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("Fan ON ? : %u\n", receive);
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("LaserDAC_ON ?: %u\n", receive);
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("FanDACVal ?: %u\n", receive);
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("LaserDACval ?: %u\n", receive);
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("LaserSwitch ?: %u\n", receive);
    usleep(10000);

    transfer_byte(fd, &data, &receive);
    printf("Gain and AutoGain Setting ?: %u\n", receive);
    usleep(10000);
}

void set_bin_index(int fd)
{
    int j = 0;
    unsigned char data1 = 0x05, data2 = 0x02, receive;
    printf("Set bin index\n");
    transfer_byte(fd, &data1, &receive);
    usleep(10000);
    while(receive != 0xF3)
    {
        usleep(10000);
        transfer_byte(fd, &data1, &receive);
        printf("Received byte: %.2X\n", receive);
        printf("Wait F3\n");
        j = j + 1;
        printf("Wait F3\n");
        if(j == 30)
        {
            printf("Too many error wait 5 s for buffer to reset: %d\n", j);
            sleep(5);
            j = 0;
        }
    }
    usleep(10000);

    printf("Ready: %u\n", receive);
    transfer_byte(fd, &data2, &receive);
    printf("0x05? %u\n", receive);
    usleep(10000);
}

void read_config_variable(int fd)
{
    int i;
    unsigned char data = 0x3C, receive;
    unsigned int bin_boun[50] = {}, bin_diameter[50] = {}, bin_wei[48] = {}, setting[20] = {};
    transfer_byte(fd, &data, &receive);
    usleep(10000);
    while(receive != 0xF3)
    {
        printf("Received byte: %.2X\n", receive);
        usleep(20);
        transfer_byte(fd, &data, &receive);
    }
    printf("Ready, Read Config Var: %.2X\n", receive);

    for(i = 0; i < 50; i++)
        {
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            bin_boun[i] = receive;
            usleep(20);
        }
    for(i = 0; i < 50; i++)
        {
            printf("Bin Boundaries ADC: %d\n ", bin_boun[i]);
        }

    for(i = 0; i < 50; i++)
        {
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            bin_diameter[i] = receive;
            usleep(20);
        }
    for(i = 0; i < 50; i++)
        {
            printf("Bin Boundaries diameter: %d\n ", bin_diameter[i]);
        }

    for(i = 0; i < 48; i++)
        {
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            bin_wei[i] = receive;
            usleep(20);
        }
    for(i = 0; i < 48; i++)
        {
            printf("Bin Weightings: %d\n ", bin_wei[i]);
        }

    for(i = 0; i < 20; i++)
        {
            transfer_byte(fd, &data, &receive);
            printf("Received byte: %.2X\n", receive);
            setting[i] = receive;
            usleep(20);
        }
    for(i = 0; i < 20; i++)
        {
            printf("Settings: %d\n ", setting[i]);
        }
}
