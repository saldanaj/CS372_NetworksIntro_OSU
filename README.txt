Student: Joaquin Saldana 
Course: CS372 / Intro to Networks 

Readme.txt file 

This file will provide specific instructions on how to compile and run the following programs 
1. chatserve.py
2. chatclient.c 

1. chatserve.py 
You will need to run the chatserve.py program first BUT before you do I do ask that you check it's an executable file first.  
In the terminal type in the command "ls -pla" and verify chatserve.py is an executable.  If not, I ask that you please run the following command: 

chmod +x chatserve.py 

This will convert it to an executable file 

After it's been converted to an executable file (if necessary) you can run the program by typing the program name plus the desired port number
to run on like below:

chatserve.py 9002 

2. chatclient.c 

You will need to compile the .c file first by typing "make" in the command line.  The Makefile will compile the program w/ the following 
command: 

gcc -g chatclient.c -o chatclient -pthread 

After the program has been compiled, you can run it by typing the program name, plus the server hostname and the port number. (**IMPORTANT**) 
Please note the chatserve.py program/server is HARD CODED to run on flip3 (**IMPORTANT**).  You may change the host if you wish to in the 
chatserve.py program, but if you do not, then you will need to communicate to that server by typing the following command line below w/ 
the chatclient program: 

chatclient flip3 9002

You will need to type in the port the chatserve.py program is running on (in this example it's port no. 9002).   




Other Information 
-----------------
After initializng both programs, the chatclient will need to initiate the chat contact w/ the server.  In other words, the chatclient will need 
to "send" the first message.

After that you can type "quit" in either the chatserve or the chatclient applications to close the socket's on both endpoints.  

Thank you.  

Joaquin    
