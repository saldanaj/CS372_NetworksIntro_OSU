#!/usr/bin/python 

# Student: Joaquin Saldana 
# Assignment 2 / CS372 Intro to Networks 
# Description: this is the python program for the file transfer client 
# which relays commands to the ft server.  The commands are either to 
# receive a list of the files in the directory OR request a file that 
# currently exists in the directory 

# much of the socket programming notes were borrowed from 
# Chap 2 of the textbook, and http://docs.python.org 

import socket 
import fileinput 
import string 
import io 
import sys
from thread import * 
from socket import *



#============================================================
# FUNCTIONS 
#============================================================

# createDataPortSocket
# This function will create the data port socket that will act as the
# server to receive the message (whether it's a file or the listing of a
# a directory from the ftserver

def createDataPortSocket(dp):

    # creating the dataport socket before we begin sending data to the ftclient
    # this is connect Q

		dataportSocket = socket(AF_INET, SOCK_STREAM)
		dataportSocket.bind(('', dp))

		dataportSocket.listen(1)
		print("Ready to listen on the dataport ... ")

		return dataportSocket

#=============================================================

# invalidCommand
# This function will notify the user they entered an invalid command on the screen
# and will close the connection and data socket's and exit w/o a 0 value.

def invalidCommand(clientSocket, dpSocket):

		dpSocket.close()
		clientSocket.close()
		sys.exit(1)

		# may need to include a return statement here (not sure) !!!!


#=============================================================

# getDirectoryList 
# This function will request a list of the  files in the server's directory 
# and will process the message returned from the server to the 
# terminal (print to the terminal)  


def getDirectoryList(clientSocket, command, dpSocket, dataport):

	# sending to the server our command -l
	fullMessage = command + " " + str(dataport)
	clientSocket.send(fullMessage)
    
   	message = clientSocket.recv(100)
        
	if message ==  "Invalid":
    
        # call the function to kill the socket's and exit w/ an error because it was an invalid command
		print("ERROR: you did not enter a valid command.  Please try again. All connections have been closed.  Called from the ")
		invalidCommand(clientSocket, dpSocket)

	else:
		
		print message
		
        # accept what the server is sending (in this case it should be the directory listing)
		dataConnectionSocket, address = dpSocket.accept()
        
        # save the text to the fileData variable
		fileData = dataConnectionSocket.recv(100000)
		directList = fileData.split()
        
        # print the directory listing to the console/terminal

		print("Below are the files located on the server's directory")

		for dirFileName in directList:
			print(dirFileName.decode())

        # close the sockets, both connection and data
		print("Closing the connection and data sockets")
		dpSocket.close()
		clientSocket.close()

	return 0

#=============================================================

# getFile
# this function will request a file present in the server's directory 
# and will process the message & file returned from the server 
# This will be done by opening a 2nd TCP connection, the data port, 
# and will place the file requested in the client's directory 


def getFile(clientSocket, command, dpSocket, dataport, filename):
	
	# sending to the server our command -g
	fullMessage = command + " " + str(dataport) + " " + filename
	clientSocket.send(fullMessage)

	message = clientSocket.recv(100)
      
	if message == "Invalid":
    
        # call the function to kill the socket's and exit w/ an error because it was an invalid command
		print("ERROR: you did not enter a valid command.  Please try again. All connections have been closed.  Called from the getFile")
		invalidCommand(clientSocket, dpSocket)
	
	elif message == "File Not Found":
	
		# the file the user was attempting to ftp was not found
		print("ERROR: File requested was not found")
		invalidCommand(clientSocket, dpSocket)
	
	else:

		print message

		# accept what the server is sending (in this case it should be a file)
		dataConnectionSocket, address = dpSocket.accept()
		
		# open the file and start receiving the data from the data socket
		dirFileName = open(filename, "w+")

		# start writing to the file and continue writing until we have reached the end of file

		while True:
			fileText = dataConnectionSocket.recv(1024)

			if not fileText:
				break

			if len(fileText) == 0:
				break
					
			dirFileName.write(fileText)
				
		# close the file & sockets, both connection and data
		dirFileName.close()
		print("Transfer complete")
		print("Closing the connection and data sockets")
		dpSocket.close()
		clientSocket.close()
	
	return 0

#=============================================================

def main(): 

	#Introduction 
	print("Welcome to the ftclient, here you can connect to an ftserver")
	
	if len(sys.argv) < 4: 
		print("Sytanx Error: You must enter at minimum the server host, server port, and command")
		sys.exit(1)

	# assigning the server name passed by the user from the command line
	servername = sys.argv[1]
	serverport = int(sys.argv[2])

	# initiating the socket for the client, this is connection P
	clientSocket = socket(AF_INET, SOCK_STREAM)
	clientSocket.connect((servername, serverport))


	# while loop that interprets the command from the user and performs the
	# necessary tasks 
	command = sys.argv[3]

    # if the length of the commands is 4, then that means the user
    # is requesting a listing of the directory (a -l command)

	if len(sys.argv) == 5:

		dataport = int(sys.argv[4])
		dpSocket = createDataPortSocket(dataport)
		getDirectoryList(clientSocket, command, dpSocket, dataport)
        
	else:
        
        # the user is reqeuesting a file from the ftserver
        # passed the -g command
        
		filename = sys.argv[4]
		dataport = int(sys.argv[5])
		dpSocket = createDataPortSocket(dataport)
		getFile(clientSocket, command, dpSocket, dataport, filename)


main()












