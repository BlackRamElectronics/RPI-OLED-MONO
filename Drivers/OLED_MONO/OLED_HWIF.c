//====================================================================================
// OLED Hardware Interface
//
// This file should be edited by the user to provide to a peripheral driver for
// interfacing to the OLED
//
// BlackRamElectronics.com
//====================================================================================

// Function decleration file
#include <stdint.h>			// Include standard types
#include "OLED_HWIF.h"

// User specific header files

void setup_io();
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

// Chenge these definitions if you use differant gpio lines
#define CD_PIN	23
#define RST_PIN	24

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

int fd;

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

//======================================================================
// SPI code
#include <stdint.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *SPI_DEVICE = "/dev/spidev0.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;

//====================================================================================
void OLED_InitIF(void)
{
	// SPI vars
	int ret = 0;

	// Set up gpi pointer for direct register access
	setup_io();

	// Switch GPIO 23 & 24 to output mode
	INP_GPIO(CD_PIN); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(CD_PIN);
    
    INP_GPIO(RST_PIN); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(RST_PIN);

	fd = open(SPI_DEVICE, O_RDWR);
	if(fd < 0)
	{
		pabort("can't open device");
	}
	
	delay = 0;
	mode |= SPI_CPHA;
	mode |= SPI_CPOL;
	
	// SPI mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if(ret == -1)
	{
		pabort("can't set spi mode");
	}

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if(ret == -1)
	{
		pabort("can't get spi mode");
	}
	
	// Bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(ret == -1)
	{
		pabort("can't set bits per word");
	}

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if(ret == -1)
	{
		pabort("can't get bits per word");
	}

	// Max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret == -1)
	{
		pabort("can't set max speed hz");
	}

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(ret == -1)
	{
		pabort("can't get max speed hz");
	}

	// Comment the following lines in if you wish to see the SPI configuration
	// printf("spi mode: %d\n", mode);
	// printf("bits per word: %d\n", bits);
	// printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

//====================================================================================
// Set up a memory regions to access GPIO
void setup_io()
{
	// Open /dev/mem
	if((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	// mmap GPIO
	gpio_map = mmap(
		NULL,					// Any adddress in our space will do
		BLOCK_SIZE,				// Map length
		PROT_READ|PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		GPIO_BASE				// Offset to GPIO peripheral
	);

   close(mem_fd); // No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED)
   {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }
//====================================================================================
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
}

//====================================================================================
void OLED_DeinitIF(void)
{
	close(fd);
}

//====================================================================================
void OLED_MsDelay(uint32_t ms_delay)
{
	usleep(ms_delay * 1000);
}

//====================================================================================
void OLED_SendByte(uint8_t data)
{
	int ret;
	
	uint8_t tx[1];
	
	tx[0] = data;
	
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	
	struct spi_ioc_transfer tr =
	{
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	
	if(ret < 1)
	{
		pabort("can't send spi message");
	}
}

//====================================================================================
void OLED_ResetAssert(void)
{
	GPIO_CLR = (1 << RST_PIN);	// Turn the OLED off
}

//====================================================================================
void OLED_ResetDeassert(void)
{
	GPIO_SET = (1 << RST_PIN);	// Turn the OLED on
}

//====================================================================================
void OLED_SetCommand(void)
{
	GPIO_CLR = (1 << CD_PIN);	// CD high
}

//====================================================================================
void OLED_SetData(void)
{
	GPIO_SET = (1 << CD_PIN);	// CD low
}

