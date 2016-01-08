#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT1 19325
#define MAXLINE 1000

//SIGCHLD handler
void signalHandler(int signal)
{
	pid_t kidpid;
    int status;
	if(signal==SIGSEGV)
	{
		printf("Wrong hostname or IP address!\n");
		abort();
	}
	if(signal==SIGABRT)
	{
		exit(signal);
	}
	if (signal==SIGCHLD) {
		//asking parent to wait for other child to terminate
		while ((kidpid = waitpid(-1, &status, WNOHANG)) > 0)
		{
			return;
		}
	}
}


int main(int argc, char **argv)
{
    int sockfd, len, flag=0, pfd[2],cStatus,n,ich,choice;
    struct hostent *hostadd;
    char str[100], *servadd, *sa, line[MAXLINE],data[300]="Child Started!",ch[100];
	pid_t pId,returnValue;
	char *cfd;
	
	//If user input wrong IP or hostname
	signal(SIGSEGV,signalHandler);
	//If program try to abort by calling abort()
	signal(SIGABRT,signalHandler);
		
	
	if(argc!=2)
	{
		fprintf(stderr,"Please enter valid destination IP\n");
		return 1;
	}
	
	//Converting IP address to hostname
	if ((inet_aton(argv[1], &servadd))!=0)
	{
		hostadd=gethostbyaddr(&servadd, sizeof(servadd), AF_INET);
		fprintf(stdout,"\nThe server hostname is:%s",hostadd->h_name);
		sa=argv[1];
	}
	
	//Converting hostname to IP address
	else 
	{  
		hostadd = gethostbyname(argv[1]);
		sa=inet_ntoa(*(struct in_addr*)hostadd->h_addr);
		fprintf(stdout,"\nThe server host IP is:%s",sa);			
    }
	 
	//whenever any child terminates by generating SIGCHLD signal
	signal (SIGCHLD, signalHandler);
	
	while(1)
	{
		
		fprintf(stdout,"\n1.Echo");
		fprintf(stdout,"\n2.Time");
		fprintf(stdout,"\n3.Quit");
		fprintf(stdout,"\nEnter the service you want(1/2/3):");
		fflush(stdin);
		gets(ch);
		choice= sscanf(ch, "%d",&ich);
		
		if(!choice)
		{
			printf("\nWrong Choice!! Try Again !");
			continue;
		}
		
		switch(ich)
		{
			case 1:	/***Echo Service***/
					if(pipe(pfd)<0)
						fprintf(stdout,"\nError in creating pipe");
					
					pId = fork();
					//Error in fork
					if(pId<0)
					{
						perror("\nCan't fork");
						break;
					}
					//Child process
					else if (pId == 0) 
					{

						close(pfd[0]);
						sprintf(cfd,"%d",pfd[1]);
						if((execlp("xterm", "xterm", "-e", "./echo_cli", "127.0.0.1", cfd, (char *) 0)) < 0)
						{
							perror("execlp");
							_exit(1);
						}
						
						close(pfd[1]);
						exit(EXIT_SUCCESS);
					}
					//Parent process
					else
					{	
						close(pfd[1]);
						
						while((n=read(pfd[0], line, sizeof(line)))>0)
						{
							fflush(stdout);
							fprintf(stdout,"\nMessage from child to parent:%s",line);
						}
						
						close(pfd[0]);
						wait(NULL);
					}
					printf("\nChild terminated!");
					break;
					
			case 2:	//***Time Service***/
					if(pipe(pfd)<0)
						fprintf(stdout,"\nError in creating pipe");
					
					pId = fork();
					
					//Error in fork
					if(pId<0)
					{
						perror("\nCan't fork");
						break;
					}
					
					//Child process
					else if (pId == 0) 
					{
						fprintf(stdout,"In Child");
						close(pfd[0]);
						sprintf(cfd,"%d",pfd[1]);
						if((execlp("xterm", "xterm", "-e", "./mytime_cli", "127.0.0.1",cfd, (char *) 0)) < 0)
						{
							perror("execlp");
							_exit(1);
						}
					
 						write(pfd[1], data, strlen(data));
						close(pfd[1]);
						exit(EXIT_SUCCESS);
					}
					
					//Parent process
					else
					{
						close(pfd[1]);
						while((n=read(pfd[0], line, sizeof(line)))>0)
						{
							fflush(stdout);
							fprintf(stdout,"\nMessage from child to parent:%s",line);
						}							
						close(pfd[0]);
						wait(NULL);
						printf("\nChild terminated!");
						
					}
					break;
			case 3:fprintf(stdout,"\nThank you for using the service..\n");
				   exit(0);
			default:fflush(stdout);
					fprintf(stdout,"\nWrong Choice!! Try Again !");	   
					break;
		}
		
   
	
  //  close(sockfd);
    }
    return 0;
}