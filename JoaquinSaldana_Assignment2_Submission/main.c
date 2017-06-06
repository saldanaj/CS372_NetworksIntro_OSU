/* 
 * Student: Joaquin Saldana 
 * Assignment 2 / CS372 Intro to Networks 
 * Description: this is the main function of the ftserver.h/.c application 
 */


#include "ftserver.h"

/*
 * Some of the socket connection information was collected from the notes 
 * for CS344 Operating Systems (Prof. Brewster) 
 */

int main(int argc, char* argv[]) 
{
    // check if the user entered a server port number 
    // if not return a message of the syntax error and 
    // exit the program w/ a 2 
    if(argc < 2)
    {
        fprintf(stderr, "Syntax error: syntax must include a server port number\n"); 
        exit(2); 
    }
    
    printf("Welcome to the ftserver program.  This program will allow you to transfer files from the current directory\n"); 
    
    int serverPortNumber = atoi(argv[1]); 
    
    if(serverPortNumber <= 1024 || serverPortNumber > 65535)
    {
        fprintf(stderr, "ERROR: must pick a port > 1024 or < 65535\n"); 
        exit(2); 
    }
    
    // variables for the socket connections 
    int listenSocketFD, establishedConnectionFD, charsRead; 
    socklen_t sizeOfClientInfo; 
    struct sockaddr_in serverAddress, clientAddress; 
    
    // buffer and pointers required for reading the incoming message 
    char recvBuffer[256]; 
    char * command; 
    char * dataport; 
    char * fileName; 
    int dp; 
    int functionResult; 
    
    // clear the server address struct 
    memset((char*) &serverAddress, '\0', sizeof(serverAddress)); 
    
    // create a network capable socket 
    serverAddress.sin_family = AF_INET; 
    
    // store the port number and convert it to host to network short 
    serverAddress.sin_port = htons(serverPortNumber); 
    
    // Any address is allowed for connection to this process 
    serverAddress.sin_addr.s_addr = INADDR_ANY;     
    
    //setup the socket 
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
    
    if(listenSocketFD < 0)
    {
        fprintf(stderr, "ERROR: unable to open the socket\n"); 
        exit(3); 
    }
    
    // now we enable the socket to being listening 
    if(bind(listenSocketFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "ERROR: occurred at the binding\n"); 
        exit(4); 
    }
    
    // listen on the port w/ a max of 5 connections 
    listen(listenSocketFD, 5); 
    
    while(1)
    {
        printf("Listening ... \n"); 
        
        // get the size of the address that is about to connect 
        sizeOfClientInfo = sizeof(clientAddress); 
        
        // accept the connection 
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *) &clientAddress, &sizeOfClientInfo); 
        
        // test if the established connection file descriptor was opened correctly 
        if(establishedConnectionFD < 0)
        {
            fprintf(stderr, "ERROR: occurred at the accepting the client's address information\n"); 
            exit(5); 
        }
        
        /*====================
         * Get the client information to pass it to the list and file 
         * functions so it can be passed to the function that creates the data 
         * connection 
         * ===================
         */
        
        // get the client information  
        char host[1024]; 
        char service[20]; 
        
        memset(host, '\0', 1024); 
        
        int getInfoResult = getnameinfo( (struct sockaddr *) &clientAddress, sizeof(clientAddress), host, sizeof(host), service, sizeof(service), 0); 
        
        if(getInfoResult != 0)
        {
            fprintf(stderr, "ERROR: occurred when calling the getnameinfo function\n"); 
            exit(5); 
        }
        
        printf("Client Host: %s\n", host); 
        printf("Client Service: %s\n", service); 
        
       // ==========================
        
        // clear the buffer 
        memset(recvBuffer, '\0', 256); 
        
        // read the clients message 
        charsRead = recv(establishedConnectionFD, recvBuffer, 255, 0); 
        
        if(charsRead < 0)
        {
            fprintf(stderr, "ERROR: error reading the file received\n"); 
            close(establishedConnectionFD);       
            close(listenSocketFD); 
            exit(6); 
        }
        if(charsRead == 0)
        {
            /*
             * MAY NEED TO DO SOMETHING HERE CLOSING THE CONNECTION W/ THE CLIENT 
             * BECAUSE A 0 FOR CHAR'S READ MEANS THE CLIENT HAS TERMINATED THE 
             * CONNECTION 
             */
            printf("The client has terminated the connection\n"); 
            
            // close the connection w/ the client address 
            close(establishedConnectionFD); 
        }
        if(charsRead > 0)
        {
            /*
             * We successfully received the message from the client 
             */
            
            printf("Client has connected\n");
            
            /*
             * Going to break apart the string received to determine what the 
             * client wants to do  
             */
                        
            command = strtok(recvBuffer, " "); 
            
            if(strcmp(command, "-l") == 0)
            {
                // the user is requesting the directory list 
                dataport = strtok(NULL, " "); 
                dp = atoi(dataport); 
                
                charsRead = send(establishedConnectionFD, "Processing your directory list request ... ", 43, 0); 
                
                // error checking 
                if(charsRead < 0)
                {
                    fprintf(stderr, "ERROR: writing to the socket\n");
                    close(establishedConnectionFD);       
                    close(listenSocketFD); 
                    exit(7); 
                }
                
                // call the sendList function 
                functionResult = sendList(host, dp); 
                
                // check if the function exited correctly
                if(functionResult < 0)
                {
                    fprintf(stderr, "ERROR: the sendList function exited with an error\n"); 
                }
                
            }
            else if(strcmp(command, "-g") == 0)
            {
                // the user is requesting a file from the server's directory 
                dataport = strtok(NULL, " "); 
                fileName = strtok(NULL, " "); 
                dp = atoi(dataport); 
                
                // MAY HAVE TO HOLD OFF ON SENDING THIS MESSAGE BECAUSE STILL HAVE THE OPTION OF SENDING 
                // FILE NOT FOUND IN CASE FILE REQUESTED DOESN'T EXISTS. 
                // charsRead = send(establishedConnectionFD, "Processing your file request ...", 32, 0);
                
                /*
                 * Check if the file they are requesting exists in the directory 
                 * If it does not, then send the appropriate message back to the ftclient 
                 */
                
                if(access(fileName, F_OK) != -1)
                {
                    charsRead = send(establishedConnectionFD, "Processing your file request ...", 32, 0);
                    
                    // error checking 
                    if(charsRead < 0)
                    {
                        fprintf(stderr, "ERROR: writing to the socket\n");
                        close(establishedConnectionFD);       
                        close(listenSocketFD); 
                        exit(7); 
                    }
                    
                    // file exists, and now we can call the sendFile function 
                    functionResult = sendFile(host, dp, fileName); 
                    
                                    // check if the function exited correctly
                    if(functionResult < 0)
                    {
                        fprintf(stderr, "ERROR: the sendFile function exited with an error\n"); 
                    }
                    
                }
                else 
                {
                    // File does not exists so we need to notify the ftclient 
                    charsRead = send(establishedConnectionFD, "File Not Found", 14, 0); 
           
                    // error checking 
                    if(charsRead < 0)
                    {
                        fprintf(stderr, "ERROR: writing to the socket\n"); 
                        close(establishedConnectionFD);       
                        close(listenSocketFD); 
                        exit(7); 
                    }
                }
                
            }
            else
            {
                // we reached this if block if the user entered an invalid command 
                // and we are now going to notify the ftclient 
                charsRead = send(establishedConnectionFD, "Invalid", 7, 0); 
            
                if(charsRead < 0)
                {
                    fprintf(stderr, "ERROR: writing to the socket\n"); 
                    close(establishedConnectionFD);       
                    close(listenSocketFD); 
                    exit(7); 
                }
                
            }
            
            // closing the established connection socket file descriptor 
            close(establishedConnectionFD);            
        }
    }
       
    close(listenSocketFD); 
    
    return 0; 
}

