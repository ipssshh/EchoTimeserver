#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define PORT1 19325
#define PORT2 19326

//A global variable sig is used to keep a track of signal() system calls
int sig=0;


//user define signal handler function
void signal_callback_handler(int signum)
{

	printf("Caught signal %d\n",signum);
	
	switch(signum)
	{
		case SIGPIPE:	sig=1;
						return;
		case SIGURG:	return;
		
	}

}


//showTime() writes the server time with a 5 seconds interval to the client ternimal
void showTime(int fd)
{
	time_t now;
	struct tm * timeinfo;
	char *str;
	
	while(1)
	{
		time ( &now);
		timeinfo = localtime ( &now );
		str=asctime(timeinfo);
		
		if (send(fd, str, strlen(str), 0) < 0) 
		{
			printf("Child ternimation: EPIPE error detected");
			return;
		}
		
		signal(SIGPIPE, SIG_IGN);
		if(sig==1)
			break;
		
		printf ( "\nThe current date and time is: %s", str);
		sleep(5);
	}
}

//echoMessage() reveives messages sent from client and echo the same to the client socket.
void echoMessage(int fd)
{
	int f,n, error;
	char str[200];
	f = 0;
        while(!f) {
			//server will try to receive message on the connected socket
           if(( n = recv(fd, str, 200,0))<0)
		   {   
				
				continue;
				//If recv sets any of the following errno because of its blocking call to nonblocking socket
				if(errno == EINTR || errno ==EAGAIN || errno==EWOULDBLOCK)
				{	
					continue;
				}
			
		  }
			else if(n==0 && f==0)
			{
				//when client child ternimates from xterm window
				printf("\nClient termination: socket read returned with value 0");
				shutdown(fd,0);
				return;
			} 
			else
				f=1;
		   

          if (f) 
			{
				//server will try to send message on the connected socket
                if (send(fd, str, n,0) < 0) 
				{
					perror("write");
                }
				else
					f=0;
			}
        
		}
		
} 
 


int main(void)
{
    int e_sockfd,t_sockfd, s2, t,n,rv,optval,fileflags;
    struct sockaddr_in e_local,t_local,remote;
	fd_set readfds;
	struct timeval tv;
	pthread_t thread_id;
	
	

	//Creating two sockets for two client services -Echo and Time
    if ((e_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
	if ((t_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
	
    e_local.sin_family = AF_INET;
  	e_local.sin_port=htons(PORT1);
	inet_pton(AF_INET, "127.0.0.1", &(e_local.sin_addr));
	
	t_local.sin_family = AF_INET;
  	t_local.sin_port=htons(PORT2);
	inet_pton(AF_INET, "127.0.0.1", &(t_local.sin_addr));
	optval = 1;
	
	//Setting the sockets to allow reuse of local addresses, if this is supported by the protocol. 
	setsockopt(e_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	setsockopt(t_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
	
	
	//Binding the socket to the local address structure.
    if (bind(e_sockfd, (struct sockaddr *)&e_local, sizeof(e_local)) == -1) {
        perror("bind");
        exit(1);
    }
	
	//Making the bounded socket non blocking by setting flags 
	if ((fileflags = fcntl(e_sockfd, F_GETFL, 0)) == -1) 
	{
		perror("fcntl F_GETFL");
		exit(1);
	}
	if ((fcntl(e_sockfd, F_SETFL, fileflags | O_NONBLOCK)) == -1) 
	{
		perror("fcntl F_SETFL, FNDELAY");
		exit(1);
	}
			
	
	//Binding the socket to the local address structure.
	if (bind(t_sockfd, (struct sockaddr *)&t_local, sizeof(t_local)) == -1) 
	{
        perror("bind");
        exit(1);
    }
	//Making the bounded socket non blocking by setting flags 
	if (fileflags = fcntl(t_sockfd, F_GETFL, 0) == -1) 
	{
		perror("fcntl F_GETFL");
		exit(1);
	}
	if (fcntl(t_sockfd, F_SETFL, fileflags | O_NONBLOCK) == -1) 
	{
		perror("fcntl F_SETFL, FNDELAY");
		exit(1);
	}
			
	//listen to enable connection requests on the socket
    if (listen(e_sockfd, 5) == -1) 
	{
        perror("listen");
        exit(1);
    }
	
	
	if (listen(t_sockfd, 5) == -1) 
	{
        perror("listen");
        exit(1);
    }

	FD_ZERO(&readfds);

	


	while(1)
	{
		FD_SET(e_sockfd, &readfds);
		FD_SET(t_sockfd, &readfds);

		n = (e_sockfd>t_sockfd)? e_sockfd:t_sockfd + 1;
		t = sizeof(remote);
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		//printf("**");
		
		//select will check for multiple sockets if anyone of them have data to send
		rv = select(n, &readfds, NULL, NULL, &tv);
	
		if (rv == -1) 
		{
			//error in select
			perror("select"); 
		} 
		else if (rv == 0) 
		{
			//waiting for connection
			printf(".");
			fflush(stdout);
		}
		else 
		{
			// one or both of the descriptors have data
			if (FD_ISSET(e_sockfd, &readfds)) 
			{
				//accepting connection from a remote address 
				if ((s2 = accept(e_sockfd, (struct sockaddr *)&remote, &t)) == -1) 
				{
					perror("accept");
					exit(1);
				}
				printf("\nconnected to echo client");
				
				//Making the listenning socket block by resetting the flags
				if (fileflags = fcntl(e_sockfd, F_GETFL, 0) == -1) 
				{
					perror("fcntl F_GETFL");
					exit(1);
				}
		
				if (fcntl(e_sockfd, F_SETFL, fileflags & (~O_NONBLOCK)) == -1) 
				{
					perror("fcntl F_SETFL, FNDELAY");
					exit(1);
				}
			
			
				//Creating independent thread to serve the client request for Echo
				if( pthread_create( &thread_id , NULL ,  echoMessage , (void*)s2) < 0)
				{
					perror("could not create thread");
				
				}
				//Let all the threads to continue execution in parallel 	
				pthread_detach(thread_id);
				
				
			}
    
			if (FD_ISSET(t_sockfd, &readfds)) 
			{
				//accepting connection from a remote address 
				if ((s2 = accept(t_sockfd, (struct sockaddr *)&remote, &t)) == -1) 
				{
					perror("accept");
					exit(1);
				}	
				printf("\nconnected to time client");
			
				//Making the listenning socket block by resetting the flags
				if (fileflags = fcntl(t_sockfd, F_GETFL, 0) == -1) 
				{
					perror("fcntl F_GETFL");
					exit(1);
				}
				
				if (fcntl(t_sockfd, F_SETFL, fileflags & (~O_NONBLOCK)) == -1) 
				{
					perror("fcntl F_SETFL, FNDELAY");
					exit(1);
				}
				
				//Creating independent thread to serve the client request for Time
				if( pthread_create( &thread_id , NULL ,  showTime , (void*)s2) < 0)
				{
					perror("could not create thread");
				
				}
				//Let all the threads to continue execution in parallel 
				pthread_detach(thread_id);
				
			

			}	
		}
	
	}	
    //Closing sockets and threads safely.
	close(s2);
	close(e_sockfd);
	close(t_sockfd);
	pthread_exit(NULL);
    return 0;
}
