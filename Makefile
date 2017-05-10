all: chatclient

chatclient: chatclient.c
	gcc -g chatclient.c -o chatclient -pthread
	#valgrind --tool=memcheck --leak-check=yes ./chatclient


clean: 
	rm -rf *.o chatclient


	
