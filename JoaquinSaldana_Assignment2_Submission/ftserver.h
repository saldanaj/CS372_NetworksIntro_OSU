/*
 * Author: Joaquin Saldana 
 * Assignment 2 / CS372 
 * Description: this is the header file to the ftserver.c file.  Here I outline the required header files 
 and define the functions headers.
 */

#ifndef FTSERVER_H
#define	FTSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>


#define MESSAGE_SIZE 100000

int createDataConnection(char * host, int dataport); 
int sendList(char * host, int dataport); 
int sendFile(char * host, int dataport, char * fileName);


#endif	/* FTSERVER_H */

