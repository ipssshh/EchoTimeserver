#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

#define PORT2 19326

//receiving servertime from Server on PORT2
void getTime(int fd, int pifd)
{
	int t;
	char str[300], erstr[300]="Server closed connection\n";
	while((t=recv(fd, str, 300, 0))>0)
	{	
		str[t] = '\0';
        printf("Server Time: %s", str);
			
    }   
    if (t < 0) 
		perror("recv"); //error in receive 
    else
	{	
		//In case of server premature shutdown 
		write(pifd, erstr, sizeof(erstr));
        exit(1);
	}
  
}

int main(int argc, char **argv)
{
    int sockfd, len, flag=0,ch, pfd[2],pifd;
    struct sockaddr_in remote;
	struct hostent *hostadd;
    char str[100], *servadd,data[100]="Hello, Server.I am time child";
	struct in_addr **addr_list;
	pid_t pId;
	
	if(argc<2)
	{
		fprintf(stderr,"Please enter destination IP");
		return 1;
	}
	
	
	//Creating client side socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

	pifd = atoi(argv[2]);
	write(pifd, data, sizeof(data)); //sending message to parent through half duplex pipe
	
	printf("\nTrying to connect...\n");

    remote.sin_family = AF_INET;
	remote.sin_port=htons(PORT2);
	inet_pton(AF_INET, argv[1], &(remote.sin_addr));
	//connecting to the remote server on PORT2
    if (connect(sockfd, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");
	//call for reveiving server time thourgh connected socket
	getTime(sockfd,pifd);	
	
	close(sockfd);
	return 0;
}