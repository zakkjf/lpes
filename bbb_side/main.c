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
#include <math.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string.h>

#include "uart_driver.h"
#include "parser.h"
#include "gps/gps_decoder.h"
#include "gps/gps_calc.h"
#include "gps/gps_dist.h"
#include "main.h"

#define DEBUG ON
#define HEARTBEAT_TEST OFF
#define SENSOR_OUTPUT OFF
#define LOGPATH "log.txt"

#define MSG_SIZE_MAX 101

#define USE_TCP 0

#define FAKE_UART 1 //switch on this mode if UART is being difficult
#define FAKE_UART_DATA "TIVA:01:1:12376$GPGGA,215907.00,4000.43805,N,10515.80958,W,1,04,9.85,1638.9,M,-21.3,M,,*5C\n\r" //MUST MATCH MSG_LEN!!!

/*DON'T MESS WITH ANYTHING BELOW THIS LINE*/
/*--------------------------------------------------------------*/

#define SRV_QUEUE_NAME "/testqueue6"
#define LOG_SIZE_MAX 256
#define QUEUE_SIZE_MAX 20
#define MSG_BUFFER_SIZE LOG_SIZE_MAX + QUEUE_SIZE_MAX
#define PERMISSION 0660

#define ON 1
#define OFF 0
#define ACTIVE ON
#define CLEAR OFF

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

char board[4];
char id, version;
uint32_t heartcounter;
imu_raw_t imu;
gps_raw_t gps;

FILE *fp;

/**
​ ​*​ ​@brief​ ​Synchronous logging call
​ ​*
​ ​*​ ​logs synchonously to specified file
​ ​*
​ ​*​ ​@param​ ​filename of log
 * @param name of thread thread currently logging
 * @param log text to log
​ *
​ ​*​ ​@return​ void
​ ​*/
int sync_logwrite(char* filename,char* log)
{
	time_t timer;
	char timebuf[25];
	//int tid, pid;
	struct tm* time_inf;

    pthread_mutex_lock(&log_mutex);

	fp = fopen (filename, "a");

	time(&timer);

	time_inf = localtime(&timer);
	strftime(timebuf, 25, "%m/%d/%Y %H:%M:%S", time_inf);

	//tid = syscall(SYS_gettid);
	//pid = getpid();

	//fprintf(fp,"%s Linux Thread ID: %d POSIX Thread ID: %d Log:%s",timebuf,tid,pid,log);

	fprintf(fp,"%s : %s",timebuf,log);

	//Flush file output
	fflush(fp);

	//Close the file
	fclose(fp);
    pthread_mutex_unlock(&log_mutex);

    return 0;
}


/**
​ ​*​ ​@brief​ ​Synchronous encapsulator for printf
​ ​*
​ ​*​ ​Mutexes printf for asynchronous call protection
 * among multiple threads
​ ​*
​ ​*​ ​@param​ ​format print formatting
 * @param ... variadic arguments for print (char *, char, etc)
​ *
​ ​*​ ​@return​ void
​ ​*/
void sync_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}

/**
​ ​*​ ​@brief​ clientside/childside mQueue message function
​ ​*
​ ​*​ ​client side of Mqueue IPC
​ ​*
​ ​*​ ​@param​ message message to send to server/parent
 *
​ ​*​ ​@return​ 0 if successful
​ ​*/
int async_log(const char *format, ...)
{
    //sync_printf("Client Thread: Active\n");
    char logbuf [LOG_SIZE_MAX];
    mqd_t qd_server;   // queue descriptors


    va_list args;
    va_start(args, format);
    // create the client queue for receiving messages from server
    vsprintf(logbuf, format, args);
    va_end(args);
    //open server message queue from client
    if((qd_server = mq_open(SRV_QUEUE_NAME, O_WRONLY)) == -1) {
        sync_printf("ERROR Client: mq_open(server)\n");
        return 1;
    }

	//send string from message struct
	if(mq_send(qd_server, logbuf, LOG_SIZE_MAX/4, 0) == -1) {
	    sync_printf("ERROR Client: Not able to send message to server\n");
	    perror("ERROR");
	}

    return 0;
}


/**
​ ​*​ ​@brief​ serverside/parentside mQueue log thread
​ ​*
​ ​*​ ​server side of Mqueue IPC
​ ​*
​ ​*​ ​@param​ message message response to client/child
 *
​ ​*​ ​@return​ 0 if successful
​ ​*/

void *logging_th()
{
 	mqd_t qd_server;   // queue descriptors

    sync_printf("Logging Thread: Active\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = QUEUE_SIZE_MAX;
    attr.mq_msgsize = MSG_BUFFER_SIZE;
    attr.mq_curmsgs = 0;

    if((qd_server = mq_open(SRV_QUEUE_NAME, O_RDONLY | O_CREAT, PERMISSION, &attr)) == -1) {
        sync_printf("ERROR Server: mq_open(server)\n");
        //return 1;
    }
    
    char buffer [MSG_BUFFER_SIZE];
    //this part could be looped for multiple clients
    while(1) {

        // get the string from client
        if(mq_receive(qd_server, buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            sync_printf("ERROR Server: mq_receive\n");
            perror("ERROR");
            //return 1;
        }

		sync_logwrite(LOGPATH,buffer);

        #if DEBUG
        	sync_printf("%s\n", buffer); //show log in terminal
        #endif

    }

    sync_printf("Server: Exiting\n");
    return 0;
}

void *navmath_th()
{
	async_log("Navmath Thread: Active\n");
	double apes[3] = {40.007035, -105.263579,1635.7}; //location fix of comparison target
	uint32_t catchcounter =1;
	gps_raw_t prevfix;
	double dir_to_target, my_dir, diff;

	while(1)
	{
		sleep(1);
		pthread_mutex_lock(&buf_mutex);
		if(catchcounter > heartcounter) //make sure heartbeat is still running
		{
			async_log("COMM ERROR: Missed heartbeat %d\n", catchcounter);
		}
		else
		{

			split_packet(bufmsg, board, &id, &version, &imu, &gps);

			async_log("Board Type: %s ID: %d Firmware version: %d.\n",board, id, version);

			//printf("%d\n",imu.z_accel);
			if(gps.fixq == 0.00f) //tiva is sending no gps data.
			{
				//no fix
				async_log("No GPS available. Waiting for fix...\n"); //fix quality ==0 means sensor fault or no fix data available
			}
			else if(gps.fixq<50) //if fixq is greater than 50, there is no way the fix readings can be accurate
			{
				//full fix
				async_log("Fix Quality: %f\n", gps.fixq); //fix quality
					//run_distances(gps,0); //just show distance/heading to APES classroom
				async_log("Distance to the APES lecture hall %f meters\n", distance(apes[0], apes[1], apes[3], gps.lat_dec_deg, gps.lon_dec_deg, gps.altitude_m, KILOM, 0)*1000);
				dir_to_target = angle(gps.lat_dec_deg, gps.lon_dec_deg,apes[0], apes[1]);
				async_log("Heading to the APES lecture hall %f degrees CW of N\n",dir_to_target);
				my_dir = angle(prevfix.lat_dec_deg, prevfix.lon_dec_deg,gps.lat_dec_deg, gps.lat_dec_deg); //take angle between consecutive fixes for present direction (ONLY WORKS IF YOU ARE MOVING QUICKLY);
				diff = dir_to_target-my_dir;
				async_log("Pointing within %f degrees of target",diff);
				memcpy(&prevfix,&gps,sizeof(gps_raw_t)); //copy data into prevdata packet
				async_log("Current time: %02d:%02d:%02d\n",(gps.utc_h+6)%24,gps.utc_m,gps.utc_s); //convert UTC to CO time
			}
			else
			{
				//time only
				async_log("Fix quality too poor for fix");
				async_log("Fix Quality: %f\n", gps.fixq); //fix quality
				async_log("Current time: %02d:%02d:%02d\n",(gps.utc_h+6)%24,gps.utc_m,gps.utc_s); //convert UTC to CO time
			}


			//antenna direction (MPU data)
			if(imu.z_accel>0)
			{
				async_log("FIX LIKELY: GPS antenna is pointing above the horizon.\n");
			}
			else
			{
				async_log("FIX UNLIKELY: GPS antenna is pointing below the horizon.\n");
			}
		}
		pthread_mutex_unlock(&buf_mutex);
		catchcounter= heartcounter+1;
	}
	return 0;
}

void *uart_commtask_th(void *ptr)
{

    async_log("Comm Thread: Active\n");
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
		#if HEARTBEAT_TEST
			sleep(2); //throws off read rates to make heartbeat error show
		#endif
#if FAKE_UART == 0
		get_uart_line(tempbuf,fd,MSG_SIZE_MAX);
#endif
		pthread_mutex_unlock(&buf_mutex);
#if FAKE_UART
		memcpy(bufmsg,FAKE_UART_DATA, MSG_SIZE_MAX);
#else
		memcpy(bufmsg,tempbuf, MSG_SIZE_MAX);
#endif
		heartcounter++;
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

	heartcounter =0;
	pthread_mutex_init(&printf_mutex, NULL);
	pthread_mutex_init(&buf_mutex, NULL);
	pthread_mutex_init(&log_mutex, NULL);


	if(pthread_create(&log_thread, NULL, (void *)logging_th, NULL)) {
		fprintf(stderr, "Error creating Thread 3\n");
		return 1;
	}

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
