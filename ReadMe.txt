Created By		:		Ipshita Singha Roy
Student Id		:		110284325
Date			:		30 September 2015
File Type		:		README
Summary			:		The aim of this program is to develop TCP socket based client and server programs using I/O
multiplexing, parent-child processes and threads to implement an echo and time server.


****************************************************:USER DOCUMENTATION:**************************************************

This program contains four files: 
1)mserver.c - It is the main server responsible for entertaining requests from the client through socket communication.
2)mclient.c - It is the main client which will be responsible for requesting services from server through child process generation.
3)echo_cli.c - It is responsible for connecting with server for requesting Echo service.
4)mytime_cli.c - It is responsible for connecting with server for requesting Time service.

To compile the program, all of these four files should be in the same folder with Makefile. The command used is:
$make

It will compile all these four files and generate executives for all of them.

To Run the program, first run the server program by the following command:
$./mserver

Secondly, to  run the client program, a hostname or IP address of the server should be passed as an argument. Following command will do:
$./mclient localhost

OR

$./mclient 127.0.0.1

Follow the program menu to request for required services Echo or Time. Input should be 1 for Echo service or 2 for Time service and 3 to exit the program.


****************************************************:TESTING DOCUMENTATION:**************************************************


These program is a TCP client server communication with two different services by the server - 1. Echo and 2. Time. The followings could be a set of examples on how to run the program with different inputs:


INPUT	:	$./mserver
EXPECTED OUTPUT	:	Server program should start

INPUT	:	$./mclient localhost
EXPECTED OUTPUT	:	Client program should start by showing IP address for localhost. A menu with different operations possible to perform is presented and asked for user choice to select the service.

INPUT	:	$./mclient 127.0.0.1
EXPECTED OUTPUT	:	Client program should start by showing hostname for 127.0.0.1. A menu with different operations possible to perform is presented and asked for user choice to select the service.

INPUT	:	Enter the service you want(1/2/3):1
EXPECTED OUTPUT :	One client child 'xterm' window will open with "client>" prompt asking for user input. Server will print "connected to echo client". Client parent will receive message from client child "Hello, Server.I am echo child". 

INPUT	:	Enter any text say "hello" to client child 'xterm' window.
EXPECTED OUTPUT:	An echo from server with prompt "server>"  will come with the same message "hello".

INPUT	:	Press Crtl+D to terminate the client child 'xterm' window with "echo" process.
EXPECTED OUTPUT:	The 'xterm' window will terminate. Parent will receive message from child saying "Child terminated!!!". Server will show "Client termination: socket read returned with value 0"	

INPUT	:	Press Crtl+C to terminate the client child 'xterm' window with "echo" process.
EXPECTED OUTPUT:	The 'xterm' window will terminate. Parent will notify that "Child terminated!". Server will show "Client termination: socket read returned with value 0"

INPUT	:	Enter the service you want(1/2/3):2
EXPECTED OUTPUT:	One client child 'xterm' window will open showing current server time with 5 seconds interval. Server will print "connected to time client". Client parent will receive message from client child "Hello, Server.I am time child". 

INPUT	:	Press Crtl+C to terminate the client child 'xterm' window with "time" process.	
EXPECTED OUTPUT:	The 'xterm' window will terminate. Parent will notify that "Child terminated!". Server will show "Child ternimation: EPIPE error detected."		

INPUT	:	Enter the service you want(1/2/3):3
EXPECTED OUTPUT:	Client will terminate by showing the message:"Thank you for using the service.."

INPUT	:	Enter the service you want(1/2/3):7
EXPECTED OUTPUT: Client will show the error message ""Wrong Choice!! Try Again !" and show the menu again.


*********************************************************************:SYSTEM DOCUMENTATION:**********************************************************************


This program has been developed to implement TCP socket based client and server programs using I/O multiplexing, child processes and threads. In the CLIENT side it covers: 

Command line arguments processing - The client is evoked with a command line argument giving either the server IP address in dotted decimal notation. The client is able to handle either mode and figure out which of the two is being passed to it. If it is given the IP address, it then prints out to the user the host name or it is given the hostname, it then prints out to the user the IP address. It is also able to detect if it’s a proper host name or not. It is done by using functions gethostbyaddr() and gethostbyname() in client mclient.c file.

The client parent - The client provides two services: echo and time in an infinite loop in which it queries the user to enter what service the user would like to request. each service create a child process to handle this request. The parent process on the other hand, enters a second loop in which it continually reads and prints out status messages received from the child via a half-duplex pipe. It is done by using pipe() and fork() functions in mclient.c file. When a child terminates it generates SIGCHLD signal which has been handled by user defined signal handler signalHandler().

The client child -The child process execs a 'xterm' to generate a separate window through which all interactions with the server and the user take place. It is done by function execlp() which takes the server IP to connect and file descriptor as an argument. It is in mclient.c file.

Service request termination and program robustness - To terminate the echo and time client, the user can type CTRL D and CTRL C. Abrupt termination has been handled in both client and server side by catching proper signals like SIGPIPE and errors like EAGAIN in server file mserver.c.

IPC using a pipe - The child uses the pipe to send status reports to the parent, which the parent prints out in its window. It has been implemented by pipe() with pipe file descriptor passed to child process and then child send messages. It has been implemented in mclient.c and echo_cli.c and mytime_cli.c.

More robustness- Code has been tried to make as robust as possible by handling - errors, including confused behaviour by the user, defining signal handler for some kernel raised and OS raised signals.

At SERVER side:

Multi-threading and server services & Relation to Inetd superserver- The server handles multiple clients using threads to handle multiple types of service: echo and time. This is implemented by pthread_create() and pthread_detach() functions in mserver.c file.

Listening on multiple services -The server creates a listening socket for each type of service that it handles, bound to the ‘well-known port’ for that service. It then uses select() function to await clients and accept() function for establishing the connection, creates a thread for the seeking service and main thread goes back to the select to await further clients. Implemented in mserver.c file.

Thread safety , Robustness, Time server implementation, Proper status messages at the server, SO_REUSEADDR socket option, Nonblocking accept has been implemented in mserver.c mostly by using send() and recv(), different error handling mechanism, showing proper status messages, calling setsockopt() and using fcntl() to nonblock the accept call respectively.






