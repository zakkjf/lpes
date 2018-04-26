/*****************************************************************************
​ ​*​ ​Copyright​ ​(C)​ ​2018 ​by​ Zach Farmer
​ ​*
​ ​*​ ​Redistribution,​ ​modification​ ​or​ ​use​ ​of​ ​this​ ​software​ ​in​ ​source​ ​or​ ​binary
​ ​*​ ​forms​ ​is​ ​permitted​ ​as​ ​long​ ​as​ ​the​ ​files​ ​maintain​ ​this​ ​copyright.​ ​Users​ ​are
​ ​*​ ​permitted​ ​to​ ​modify​ ​this​ ​and​ ​use​ ​it​ ​to​ ​learn​ ​about​ ​the​ ​field​ ​of​ ​embedded
​ ​*​ ​software.​ Zach Farmer, ​Alex​ ​Fosdick​, and​ ​the​ ​University​ ​of​ ​Colorado​ ​are​ ​not​
 * ​liable​ ​for ​any​ ​misuse​ ​of​ ​this​ ​material.
​ ​*
*****************************************************************************/
/**
​ ​*​ ​@file​ ​main.c
​ ​*​ ​@brief​ ​pThreads example source
​ ​*
​ ​*​ ​This is the source file for a three-thread PThreads example demonstrating
 * syncronous logging, printing, signal handling, file handling,
 * and CPU utilization stats
​ ​*
​ ​*​ ​@author​ ​Zach Farmer
​ ​*​ ​@date​ ​Feb 18 2018
​ ​*​ ​@version​ ​1.0
​ ​*
​ ​*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>

#include "uart_driver.h"
#include "parser.h"
#include "gps/gps_decoder.h"
#include "gps/gps_calc.h"
#include "main.h"

#define DEBUG OFF
#define SENSOR_OUTPUT OFF
#define LOGPATH "log.txt"

#define MSG_SIZE_MAX 101
#define LOG_SIZE_MAX 256

#define USE_TCP 0

#define FAKE_UART 1 //switch on this mode if UART is being difficult
#define FAKE_UART_DATA "IMUDATA:16748$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r       " //MUST MATCH MSG_LEN!!!

/*DON'T MESS WITH ANYTHING BELOW THIS LINE*/
/*--------------------------------------------------------------*/
#define ON 1
#define OFF 0
#define ACTIVE ON
#define CLEAR OFF

#define PORT 55171
#define HOSTNAME "beaglebone"

#define SER_INTERFACE "/dev/ttyO1"

#define NUM_THREADS 3

pthread_t comm_thread;
pthread_t log_thread;
pthread_t navmath_thread;

pthread_mutex_t printf_mutex;
pthread_mutex_t buf_mutex;
pthread_mutex_t log_mutex;

char bufmsg[MSG_SIZE_MAX];
char quit[NUM_THREADS];

imu_raw_t imu;
gps_raw_t gps;



void *logging_th()
{

	return 0;
}

void *navmath_th()
{
	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&buf_mutex);
		split_packet(bufmsg, &imu, &gps);
		printf("%d\n",imu.z_accel);
		pthread_mutex_unlock(&buf_mutex);
	}
	return 0;
}

void *uart_commtask_th(void *ptr)
{
	//UART SETUP STUFF
#if FAKE_UART == 0
	char tempbuf[MSG_SIZE_MAX];
	int fd = open(SER_INTERFACE, O_RDWR | O_NOCTTY | O_SYNC);
	
	if (fd < 0)
	{
	        //error_message ("error %d opening %s: %s", errno, SER_INTERFACE, strerror (errno));
	        return NULL;
	}

	set_interface_attribs (fd, B9600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (fd, 1);                // set blocking
#endif

	while(1)
	{

#if FAKE_UART == 0
		get_uart_line(tempbuf,fd,MSG_SIZE_MAX);
#endif
		pthread_mutex_unlock(&buf_mutex);
#if FAKE_UART
		memcpy(bufmsg,FAKE_UART_DATA, MSG_SIZE_MAX);
#else
		memcpy(bufmsg,tempbuf, MSG_SIZE_MAX);
#endif
		pthread_mutex_unlock(&buf_mutex);
	}
}

void *tcp_commtask_th(void *ptr)
{

	return 0;
}

int main()
{
	quit[0] = 1;
	quit[1] = 1;
	quit[2] = 1;

	pthread_mutex_init(&printf_mutex, NULL);
	pthread_mutex_init(&buf_mutex, NULL);
	pthread_mutex_init(&log_mutex, NULL);


#if USE_TCP
	if(pthread_create(&comm_thread, NULL, (void *)tcp_commtask_th, NULL)) {
		fprintf(stderr, "Error creating Comm Thread\n");
		return 1;
	}
#else
	if(pthread_create(&comm_thread, NULL, (void *)uart_commtask_th, NULL)) {
		fprintf(stderr, "Error creating Comm Thread\n");
		return 1;
	}
#endif

	if(pthread_create(&log_thread, NULL, (void *)logging_th, NULL)) {
		fprintf(stderr, "Error creating Thread 3\n");
		return 1;
	}

	if(pthread_create(&navmath_thread, NULL, (void *)navmath_th, NULL)) {
		fprintf(stderr, "Error creating Thread 3\n");
		return 1;
	}


	if(pthread_join(log_thread, NULL)) {
        printf("Socket Thread Exited Safely");
    }

    if(pthread_join(comm_thread, NULL)) {
        printf("Socket Thread Exited Safely");
    }

    if(pthread_join(navmath_thread, NULL)) {
        printf("Socket Thread Exited Safely");
    }

	return 0;
}
