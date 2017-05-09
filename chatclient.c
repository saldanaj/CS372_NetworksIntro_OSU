/*
 Student: Joaquin Saldana 
 Course: CS372 / Intro to Networks 
 
 Description: 
 This is the "chat client" program for Assignment 1.  This program is meant to act as the client to
 a chat program.  
 
 In order to connect to the server you will need to enter the hostname and port number on the command 
 line when you run the executable file.
 */


// sections of this code were directly sourced from Beej's Guide to Network Programming
// http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

// will need to send a max number of 500 characters, meaning
// we will need a max of 500 data bytes
#define MAXDATASIZE 500
#define TRUE 1
#define FALSE 0

// function prototype
// This function is a direct suggestion from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#connect
// int sendall(int socketFD, char * buff, int *len);


void* receiveMessage(void * numberReturned)
{
    int socketFD = *((int *) numberReturned);
    
    // buffer to hold the message received
    char messageReceived[511];
    
    // set the buffer to contain null terminators
    memset(messageReceived, '\0', sizeof(messageReceived));
    
    // variables within the function itself
    int quit = FALSE;
    int charsRead;
    
    // infinite while loop that checks for the following
    // 1. if the number of bytes received is less than 0 it means there was an error
    // 2. if the message received was "quit" then it initiates a graceful shutdown of the chat client
    // 3. else, just print the message received to the console
    while(quit == FALSE)
    {
        charsRead = recv(socketFD, &messageReceived, sizeof(messageReceived) - 1, 0);
        
        if(charsRead < 0)
        {
            error("Error receiving the message from the server");
        }
        
        // In the event that the chat server sends a quit to terminate the chat
        if(strcmp(messageReceived, "/quit\0") == 0)
        {
            printf("\nChat has been terminated\n");
            send(socketFD, "/quit", 5, 0);
            quit = TRUE;
            exit(0);
        }
        else
        {
            // print the message received from the server
            printf("%s\n", messageReceived);
        }
    }
}

void sendMessage(int socketFD, char * clientHandle);

int main(int argc, char * argv[])
{
    
    // Error checking that the user entered the necesary arguments
    // such as the host name and port number
    if(argc < 3)
    {
        fprintf(stderr, "Syntax error: Syntax must include a host name and port number\n");
        exit(2);
    }
    
    //variables unique to this program
    char handleName[11];
    
    // Introduction to the program
    // Here we will greet the user and ask he enter his handle
    printf("Welcome to the chat client.  We hope you find it entertaining for both you and your friend.\n");
    printf("Please enter your handle: ");
    scanf("%s", handleName);
    strcat(handleName, ": ");
    
    // variables that will hold file descriptors
    // int quit = TRUE;
    int sockfd, portNumber, charsWritten, charsRead;
    char buffer[MAXDATASIZE];
    char tempBuffer[MAXDATASIZE];
    
    // struct that will be used to initiate the socket
    struct sockaddr_in serverAddress;
    struct hostent * serverHostInfo;
    
    // convert port number entered from string to integer
    portNumber = atoi(argv[2]);
    
    // Another pleasantry to the user
    printf("Welcome to the chat %s now let's try and connect you to \"%s\" at port %d\n", handleName, argv[1], portNumber);
    
    // setting up the server address struct
    
    // clearing out the address struct
    memset((char *)&serverAddress, '\0', sizeof(serverAddress));
    
    // creating a TCP connection
    serverAddress.sin_family = AF_INET;
    
    // storing the port number, but port number "host to network short"
    serverAddress.sin_port = htons(portNumber);
    
    // convert the host information
    serverHostInfo = gethostbyname(argv[1]);
    
    // error checking
    if(serverHostInfo == NULL)
    {
        fprintf(stderr, "Error: No such host\n");
        exit(3);
    }
    
    // copy the host information into ther serverAddress struct
    memcpy((char *)&serverAddress.sin_addr.s_addr, (char *)serverHostInfo->h_addr, serverHostInfo->h_length);
    
    // setting up of the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // error checking the socket
    if(sockfd < 0)
    {
        printf("Error occured when trying to open the socket\n");
        exit(3);
    }
    
    // try and connect to the server, if unsuccessful a error will be printed and program will terminate
    if(connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("Error occured when trying to connect to server\n");
        exit(3);
    }
    
    printf("We are now connected to the server ... \n");
    
    
    /*
    
    pthread_t messageReceivedID;
    
    int n = pthread_create(&messageReceivedID, NULL, receiveMessage, (void*) &sockfd);

    if(n)
    {
        printf("Error creating mesasge receive thread");
        exit(1);
    }
    
    sendMessage(sockfd, handleName); 
    */
    
    
    // SENDING DATA TO HOST
    
    memset(buffer, '\0', sizeof(buffer));
    memset(tempBuffer, '\0', sizeof(tempBuffer));
    printf("%s", handleName);
    fgets(tempBuffer, sizeof(tempBuffer) - 1, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(buffer, handleName);
    strcat(buffer, tempBuffer);
    
    charsWritten = send(sockfd, buffer, strlen(buffer), 0);

    // RECEIVING DATA FROM HOST
    
    memset(buffer, '\0', sizeof(buffer));
    
    charsRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    printf("%s", buffer);

    close(sockfd);
    
    printf("Thank you for using this chat program.  Goodbye!\n");
    
    return 0;
}

// ==================================
// FUNCTIONS
// ==================================

/*
 Function: receiveMessage(void* numberReturned)
 Arguments: Void * , later casted as an int within the function
 Return: N/A
 Description: this function incorporates an infinite loop to receive chat messages from
 the chatserver until the chat is ended
 */

/*
void* receiveMessage(void * numberReturned)
{
    int socketFD = *((int *) numberReturned);
    
    // buffer to hold the message received
    char messageReceived[511];
    
    // set the buffer to contain null terminators
    memset(messageReceived, '\0', sizeof(messageReceived));
    
    // variables within the function itself
    int quit = FALSE;
    int charsRead;
    
    // infinite while loop that checks for the following
    // 1. if the number of bytes received is less than 0 it means there was an error
    // 2. if the message received was "quit" then it initiates a graceful shutdown of the chat client
    // 3. else, just print the message received to the console
    while(quit == FALSE)
    {
        charsRead = recv(socketFD, &messageReceived, sizeof(messageReceived) - 1, 0);
        
        if(charsRead < 0)
        {
            error("Error receiving the message from the server");
        }
        
        // In the event that the chat server sends a quit to terminate the chat
        if(strcmp(messageReceived, "/quit\0") == 0)
        {
            printf("\nChat has been terminated\n");
            send(socketFD, "/quit", 5, 0);
            quit = TRUE;
            exit(0);
        }
        
        // print the message received from the server
        printf("%s\n", messageReceived);
    }
}
 */


/*
 Function: sendMessage(int socketFD, char * clientHandle)
 Arguments: socketFD - the socket file descriptor returned from creating the socket, & 
            char * clientHandle - reference to char array holding the client's chat handle
 Return: N/A
 Description: this function incorporates an infinite loop to send messages to the chat server.
 */

void sendMessage(int socketFD, char * clientHandle)
{
    // input buffer declarations
    char message[strlen(clientHandle) + 500], userInput[500];
    
    // function variables
    int quit = FALSE;
    int charsWritten;
    
    while(quit == FALSE)
    {
        // set null terminators in all of the buffers declared earlier
        memset(message, '\0', sizeof(message));
        memset(userInput, '\0', sizeof(userInput));
        
        // get the user input from stdin
        fgets(userInput, sizeof(userInput) - 1, stdin);
        strcat(userInput, '\0');
        
        // set up the mesasge to send
        strcpy(message, clientHandle);
        strcat(message, userInput);
        
        // check if user wants to quit
        if(strcmp(userInput, "/quit\n") != 0)
        {
            charsWritten = send(socketFD, message, strlen(message), 0);
            
            if(charsWritten < 0)
            {
                printf("Error sending message to server");
                quit = TRUE;
            }
        }
        else
        {
            printf("\nChat has been terminated\n");
            send(socketFD, "/quit\n", 5, 0);
            quit = TRUE;
        }
    }
}

/*
 // This function attempts to ensure all of the characters entered are sent to the server
 
 int sendall(int s, char * buff, int *len)
 {
 int total = 0;
 int bytesLeft = *len;
 int n;
 
 
 while(total < *len)
 {
 n = send(s, buff+total, bytesLeft, 0);
 
 if(n == -1)
 {
 break;
 }
 
 if(n < *len)
 {
 printf("We only sent a partial portion of the message.  We will try to finish sending the rest\n");
 }
 
 total+=n;
 bytesLeft -=n;
 }
 
 *len = total;
 
 return n==-1? -1:0;
 }
 
 
 
 // previously in my main function 
 
 // while the user has not typed "\quit" into the command line
 while(quit == TRUE)
 {
 // make sure to enter null terminators in the char array
 memset(buffer, '\0', sizeof(buffer));
 
 // have the user enter the text they wish to send to the server/host b
 printf("%s: ",handleName);
 scanf("%s", buffer);
 
 if(strcmp(buffer, "\\quit") == 0)
 {
 // terminate the infinite while loop
 quit = FALSE;
 }
 else
 {
 // get the users input
 fgets(buffer, sizeof(buffer) - 1, stdin);
 //remove the trailing "enter" which is the \n char
 buffer[strcspn(buffer, "\n")] = '\0';
 
 if(sendall(sockfd, buffer, strlen(buffer)) < 0)
 {
 error("Error writing to socket\n");
 }
 
 // now get the return message
 memset(buffer, '\0', sizeof(buffer));
 
 // read data from the socket
 charsRead = recv(sockfd, buffer, sizeof(buffer) - 1 , 0);
 
 printf("Host B: %s", buffer);
 }
 
 }
 
 
 
 
 
 */
































