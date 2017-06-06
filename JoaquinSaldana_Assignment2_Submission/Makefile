all: Assignment2
	
Assignment2: ftserver.o main.o
	gcc -g -Wall -o ftserver ftserver.o main.o
	#valgrind --tool=memcheck --leak-check=yes ./ftserver

ftserver.o: ftserver.h ftserver.c 
	gcc -g -Wall -c ftserver.c
	
main.o: ftserver.h ftserver.c 
	gcc -g -Wall -c main.c 
	

clean:
	rm -rf *.o ftserver