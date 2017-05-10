/*
 Student: Joaquin Saldana 
 Course: CS372 / Intro to Networks 
 
 Description: This is the "chat client" program for Assignment 1.  This program is meant to act as the client to
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
#define MAXDATASIZE 515
#define TRUE 1
#define FALSE 0


// =====================================================
// FUNCTION PROTOTYPES
// This function is a direct suggestion from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#connect

void getHandle(char * handle);
void chat(int socketFD, char * handle);

// =====================================================

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
    memset(handleName, '\0', sizeof(handleName));

    
    // Introduction to the program
    // Here we will greet the user and ask he enter his handle
    printf("Welcome to the chat client.  We hope you find it entertaining for both you and your friend.\n");
    
    getHandle(handleName);
    
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
    
    chat(sockfd, handleName);
    
    return 0;
}

// ==================================
// FUNCTIONS
// ==================================

/*
 Function: getHandle(char * handle)
 Arguments: char * - the handle char array declared in main int
 Return: N/A
 Description: this function prompts the user to enter the user handle they wish to use 
 for the chat client
 */

void getHandle(char * handle)
{
    // inner function variables
    int validInput = FALSE;
    char * position;

    // while loop used to check that the user entered a usable handle
    while(validInput == FALSE)
    {
        // Here we will greet the user and ask he enter his handle
        printf("Please enter your handle: ");
        fgets(handle, 11, stdin);

        //        if((position = strchr(handle, '\0')) == NULL)

        char test = handle[10];
        
        if(test != '\0')
        {
            printf("ERROR: handle is too long.  Max length is 10 characters\n");
            memset(handle, '\0', sizeof(handle));
        }
        else
        {
            validInput = TRUE;
            
            // changes the last character from a new line character to a NULL character
            // if there is a new line character.  If there isn't it means the user
            // used the entire handle buffer
            if((position = strchr(handle, '\n')) != NULL)
            {
                *position = '\0';
            }
        }
    }
}

/*
 Function: chat(int socket, char * handle)
 Arguments: int socket file descripter, char pointer to user's handle
 Return: N/A
 Description: this function will operate the writing and receiving of messages between the sockets
 connected for the chat program
 */

void chat(int socketFD, char * handle)
{
    // inner function variables
    ssize_t numberOfBytes;
    char receivedMessage[MAXDATASIZE];
    char input[MAXDATASIZE];
    char sentMessage[MAXDATASIZE];
    int validInput = FALSE;
    int quit = FALSE;
    int temp = 0;
    char * terminate = ": \quit";
    char * terminateSent = "quit";
    
    // Infinite loop that only terminates upon the user writing in quit
    while(quit != TRUE)
    {
        // set all of the buffer's to have null terminaters
        memset(input, '\0', MAXDATASIZE);
        memset(sentMessage, '\0', MAXDATASIZE);
        
        // check that the user did not input a much larger text than allowed
        while(validInput != TRUE)
        {
        
            printf("%s: ", handle);
            fgets(input, MAXDATASIZE, stdin);
            
            temp = strlen(input);
            
            if(temp > MAXDATASIZE)
            {
                printf("Error: Your input is too large\n");
            }
            else
            {
                validInput = TRUE;
            }
        }
        
        // reset the boolean value
        validInput = FALSE;
        
        // take out the last character which is a new line character
        // to a null terminator
        char *position;
        
        if((position = strchr(input, '\n')) != NULL)
        {
            *position = '\0';
        }
        
        temp = strlen(input);

        // going to check if the string entered by the client
        // is intenting to "quit" the chat program
        const char s[2] = "\n";
        char * subString_1 = strtok(input, s);
        
        // check if the user want's to quit the chat
//        if(strcmp(input, terminate) == 0)
        if(strcmp(subString_1, terminateSent) == 0)
        {
            printf("Connection closed by client\n");
            
            numberOfBytes = write(socketFD, input, temp);
            
            quit = TRUE;
            
            break;
        }
        
        // put together the message destined for the other endpoint
        temp = sprintf(sentMessage, "%s: %s\n", handle, input);
        
        // check for error when concatenating
        if(temp < 0)
        {
            printf("Error when concatenating send message together\n");
        }
        
        // write to the other socket
        numberOfBytes = write(socketFD, sentMessage, temp);
        
        if(numberOfBytes < 0)
        {
            printf("Error when writing/sending to the server\n");
        }
        
        // ==================================================
        // RECEIVING MESSAGE portion of the code
        
        memset(receivedMessage, '\0', MAXDATASIZE);
        
        numberOfBytes = recv(socketFD, receivedMessage, MAXDATASIZE, 0);
        
        // going to break apart the string received to see if the server want's to quit
        char * subString = strchr(receivedMessage, ':');
        subString = strtok(subString, s);
        
        if(numberOfBytes < MAXDATASIZE)
        {
            if(strcmp(subString, terminate) == 0)
            {
                printf("Server requested to quit the chat\n");
                quit = TRUE;
            }
            else if(numberOfBytes == -1)
            {
                printf("Received message error when receiving from server\n");
            }
            else if(0 < numberOfBytes < MAXDATASIZE)
            {
                printf("%s\n",receivedMessage);
            }
            else if(numberOfBytes == 0)
            {
                printf("End of server data\n");
                quit = TRUE;
            }
        }
        
    }
    
    close(socketFD);
    
    return;
    
}
