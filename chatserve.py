#!/usr/bin/python 

import socket
import fileinput  
import string 
import io
import sys 
from thread import * 

# Student: Joaquin Saldana 
# Class: CS372 - Intro to Networks 
# Assignment 1 

# Description: this is the chatserve program that acts as the chat server 
# for the chat program. The user will need to disclose what port they wish 
# the chatserver to run on.  This chatserve.py program will run on 
# the flip3 server. 

# ================================================
# Functions 
# ================================================

# receiveMessage
# This function runs on an infinite loop receiving messages from the client 
# until the chat is terminated 
def receiveMessage(connectionSocket):
    
	# receive the messages
	messageReceived = connectionSocket.recv(511)
	if(messageReceived == 0):
        	print("Error in receiving message from client")
        	return -1

    	if(messageReceived == "/quit"):
		print("Chat server has been terminated")
        	return 1
        	#connectionSocket.close()
		
	print messageReceived
    	return 0

# ==============================================

# sendMessage 
# This function runs on an infinite loop sending messages to the client 
# unti lthe chat is terminated 
def sendMessage(connectionSocket, serverHandle):

	validInput = False
    
    	while(validInput != True):

        	# get input from the user
        	inputByUser = raw_input(serverHandle + ": ")
        
        	#ask the user to re-enter the input if the user went over the amount of characters allowed
        	if(len(inputByUser) < 500):
            		validInput = True

		# concatenate the message w/ the serverHandle, the chat delimiter
		# and the user's input
		messageToSend = serverHandle + ": " + inputByUser + "\0"
	 
		if(inputByUser == "/quit"):
			connectionSocket.send(messageToSend)
        		print("Connection has been terminated by the server")
        		return 1
        
        	# connectionSocket.send("/quit\0")
	
		# send the message from the server user
		connectionSocket.send(messageToSend)
    	return 0

#=============================================

def chat(connectionSocket, serverHandle):

	#boolean value to determine when to terminate the loop
    	quit = False

    	# infinite while loop until the someone chooses to "/quit" the program
    	while(quit == False):

        	# call the receiveMessage function to write to console what the client wrote to us
        	returnedValue = receiveMessage(connectionSocket)

        	#check the value returned to see if the function succeeded in receiving the message
        	if(returnedValue == -1):
            		break
        	if(returnedValue == 1):
            		quit = True
            		connectionSocket.close()
            		break

        	# now call the sendMessage function to communicate back w/ the client
        	returnedValue = sendMessage(connectionSocket, serverHandle)

        	# check if the value returned means our server user want's to quit the program
        	if(returnedValue == 1):
            		quit = True
            		connectionSocket.close()
            		break

    	return returnedValue

#=============================================


def main():


   	# Introduction to the user, where we ask the user to enter the chat_server handle he wishes to use
    	print("Welcome to the chat server program.  We hope you find it entertaining for both you and your friend\n")
    	serverHandle = raw_input("Please enter your handle: ")

    	# check that the user entered a handle no more than 10 chars
    	while(len(serverHandle) > 10):
        	print("ERROR: Your handle needs to be 10 characters or less")
        	serverHandle = raw_input("Please enter your handle: ")


    	# assign the port number from the command line arguments passed and
    	# ensure it's stored as an int by casting it
    	portNumber = int(sys.argv[1])
    	print("We are assigning port number " + str(portNumber) + " to the socket")

    	# initiating the socket as written in chapter 2 of the book
    	from socket import *
    	serverSocket = socket(AF_INET, SOCK_STREAM)
    	serverSocket.bind(('flip3',portNumber))
    	serverSocket.listen(5)
    	print('Server is ready to send/receive messages. Listening .... ')

    	# now we enter an infinite while loop that will multithread to accept and send
    	# messages simutaneously
    	serverOpen = True
    	while(serverOpen == True):
	
        	connectionSocket, addr = serverSocket.accept()
	
        	#start_new_thread(receiveMessage, (connectionSocket,))
        	#start_new_thread(sendMessage, (connectionSocket, serverHandle))
	
        	#receiveMessage(connectionSocket)
        	#sendMessage(connectionSocket, serverHandle)


        	value = chat(connectionSocket, serverHandle)

        	if(value == 1):
            		serverOpen = False
        	else:
           		 print("Server is ready to send/receive messages.  Listening .... ")

    	serverSocket.close()

main()






