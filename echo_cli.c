#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

#define PORT1 19325

//sending messages and getting reply back from server
 void sendMessage(int fd, int pfd )
{
	int t;
	char str[200], errmsg[300]="Child Terminated !!!\n";
	while(printf("Client> "), fgets(str, 200, stdin), !feof(stdin)) {
        if (send(fd, str, strlen(str), 0) == -1) {
            perror("send"); //error in sending 
            exit(1);
        }

        if ((t=recv(fd, str, 100, 0)) > 0) {
            str[t] = '\0';
            printf("Server> %s", str);
        } else { //error in receive 
            if (t < 0) 
			{
				//In case of server premature shutdown 
				write(pfd, "Server closed \n", sizeof("Server closed \n"));
				if(errno==131)
				{
					//connection reset by peer has been taken care of
					write(pfd,"Server closed connection\n",sizeof("Server closed connection\n"));
					exit(0);
				}
			}
            
        }
    }
	//sending message to parent through half duplex pipe
	write(pfd,errmsg,sizeof(errmsg));
} 


int main(int argc, char **argv)
{
    int sockfd, len, flag=0,ch,pifd;
    struct sockaddr_in remote;
	struct hostent *hostadd;
    char str[100], *servadd, *sa, data[100]="Hello, Server.I am echo child", c;
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
	remote.sin_port=htons(PORT1);
	inet_pton(AF_INET, argv[1], &(remote.sin_addr));
	//connecting to the remote server on PORT1
    if (connect(sockfd, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");
	//call for sending and reveiving messages thourgh socket from server
	sendMessage(sockfd,pifd);
	
	 
	close(sockfd);
	return 0;
}