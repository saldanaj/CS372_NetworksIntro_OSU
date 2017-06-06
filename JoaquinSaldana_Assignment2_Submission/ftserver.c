/*
 * Author: Joaquin Saldana 
 * Assignment 2 / CS372 
 * Description: this is the C file for the ftserver application 
 * In this file we define the functions that will create the 2nd TCP connection
 * (connection Q), functions that will send the directory list and file name 
 * requested, and report any errors along the way.  
 * 
 * It should be noted some of the socket creation code was learned and borrowed 
 * from Prof. Ben Brewsters's CS344 Notes in Block 2 and 4.  
 */

#include "ftserver.h"

/*
 * Function: createDataConnectin 
 * Parameter(s): int dataport
 * Return value: int - this int will point to the socketFD that was created by 
 * the function 
 * Description: this creates the socket connection that will act as "Connection 
 * P", where the ftserver will act as the client and send the data to the ftclient 
 * acting as the server via TCP 
 */

int createDataConnection(char * host, int dataport)
{
        // declare the socket variables
        struct sockaddr_in serverAddress;
	struct hostent *serverHostInfo;   
    
        // clear out the struct 
        memset(&serverAddress, '\0', sizeof(serverAddress));
        
        // do the regular strut initializations necessary to create the 
        // data connection TCP connection 
        serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(dataport);
        
        serverHostInfo = gethostbyname(host);
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
        
        int dataConnectionSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
        
        if(dataConnectionSocketFD < 0)
        {
            fprintf(stderr, "ERROR: creating socketFD for data connection\n"); 
            return -1; 
        }
        
        if(connect(dataConnectionSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        {
            fprintf(stderr, "ERROR: connecting socketFD for data connection\n"); 
            return -2; 
        }
        
        return dataConnectionSocketFD; 
}

/*
 * Function: sendList
 * Parameter(s): int dataport
 * Return value: int (determine if program executed correctly
 * Description: this function is called when the user wants the ftserver to 
 * send a list of the files in the  directory 
 */ 

int sendList(char * host, int dataport)
{
    // declare the message buffer that will be used to transmit the 
    // file via socket 
    char dataMessage[MESSAGE_SIZE]; 
    memset(dataMessage, '\0', sizeof(dataMessage)); 
    
    // int that will hold the number of chars sent 
    int charsWritten; 
    
    // create and assign the file descriptor for the 
    // data socket used to transmit the file to the client 
    int dataSocketFD = createDataConnection(host, dataport); 
    
    if(dataSocketFD < 0)
    {
        fprintf(stderr, "ERROR: call to create data connection returned a failure\n"); 
        return -5; 
    }
    
    // declare the directory struct for which we will use to 
    // access the directory information 
    DIR * directory; 
    struct dirent * dir_entry; 
    
    directory = opendir("."); 
    
    if(directory == NULL)
    {
        fprintf(stderr, "Could not open the current working directory\n"); 
        close(dataSocketFD); 
        return -3; 
    }
    
    while(dir_entry = readdir(directory))
    {
        if(dir_entry->d_ino != 0)
        {
            strcat(dataMessage, dir_entry->d_name); 
            strcat(dataMessage, " "); 
        }
    }
    
    charsWritten = send(dataSocketFD, dataMessage, strlen(dataMessage), 0); 
    
    if(charsWritten < 0)
    {
        fprintf(stderr, "ERROR: Error writing to the data connection socket\n"); 
        closedir(directory); 
        close(dataSocketFD); 
        return -6; 
    }
        
    //  close the dataSocket file descriptor and the file stream 
    closedir(directory); 
    close(dataSocketFD); 
    
    return 0; 
}

/*
 * Function: sendList
 * Parameter(s): int dataport, char * fileName (name of the file requested) 
 * Return value: int (determine if program executed correctly
 * Description: this function is called when the user wants the ftserver to send 
 * a file in the directory.  
 */ 

int sendFile(char * host, int dataport, char * fileName)
{
    
    // create and assign the file descriptor for the 
    // data socket used to transmit the file to the client 
    int dataSocketFD = createDataConnection(host, dataport); 
    
    if(dataSocketFD < 0)
    {
        fprintf(stderr, "ERROR: call to create data connection returned a failure\n"); 
        return -5; 
    }
    
    
    /*
    // Opening the file and testing if the file opened correctly 
    int file_descriptor; 
    */
    
    // Opening the file and testing if the file opened correctly 
    FILE * fp; 
    int fileSize; 
    int charsWritten; 
    
    fp = fopen(fileName, "r"); 
    
    // error check if the file did not open correctly 
    if(fp == NULL)
    {
        fprintf(stderr, "ERROR: could not open the file\n"); 
        return -6; 
    }
    
    // seek to the end of the file and find the total file size 
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp); 
    
    // variables for reading the file 
    size_t lengthRead = 0; 
    ssize_t lineSize; 
    char * charLine; 
    
    while((lineSize = getline(&charLine, &lengthRead,fp)) != -1)
    {
        charsWritten = send(dataSocketFD, charLine, lineSize, 0); 
    
        if(charsWritten < 0)
        {
            fprintf(stderr, "ERROR: Error writing to the data connection socket\n"); 
            fclose(fp); 
            close(dataSocketFD); 
            return -7; 
        }
    }

    // close the file stream and the socket file descriptor 
    free (charLine); 
    fclose(fp); 
    close(dataSocketFD);     
    
    return 0; 
}

