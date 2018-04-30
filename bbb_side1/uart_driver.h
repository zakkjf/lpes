//Adapted from https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
//by Zach Farmer

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

#ifndef UART_DRIVER_H_INCLUDED
#define UART_DRIVER_H_INCLUDED
int set_interface_attribs (int fd, int speed, int parity);
int get_uart_line(char* buf, int fd, int buflen);
int set_blocking (int fd, int should_block);
#endif

