#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>

#define MAX 80
#define PORT 43454
#define SA struct sockaddr

void func(int sockfd){
	char buff[MAX];
	char dariClient[MAX];
	bzero(buff,MAX);
	strcpy(buff, "# munin node at MyComputer\n");
	write(sockfd,buff,sizeof(buff));
	for(;;)
	{
		bzero(buff,MAX);
		read(sockfd,buff,sizeof(buff));
		strcpy(dariClient, buff);
		printf("From client: %s\n",dariClient);
		bzero(buff,MAX);
		strcpy(buff, "# munin node at MyComputer\n");
		if(strncmp("cap", dariClient, 3) == 0){		
			strcpy(buff, "cap multigraph dirtyconfig\n");
		}
		else if (strncmp("nodes", dariClient, 5) == 0){
			strcpy(buff, "MyComputer\n");
		}
		else if (strncmp("memory", dariClient, 6) == 0){
			
			strcpy(buff, "used.value ");
			FILE *fp = popen("free | awk 'FNR == 2 {print $3}'", "r");
			if (fp == NULL) {
				printf("Failed to run command\n" );
				exit(1);
			}
			char used_buff[MAX];
			fgets(used_buff, sizeof(used_buff)-1, fp);
			pclose(fp);
			strcat(buff, used_buff);
			strcat(buff, "free.value ");
			bzero(used_buff, MAX);
			fp = popen("free | awk 'FNR == 2 {print $4}'", "r");
			if (fp == NULL) {
				printf("Failed to run command\n" );
				exit(1);
			}
			fgets(used_buff, sizeof(used_buff)-1, fp);
			pclose(fp);
			strcat(buff, used_buff);
		}
		//while((buff[n++]=getchar())!='\n');
		write(sockfd,buff,sizeof(buff));
		if(strncmp("exit",buff,4)==0){
			printf("Server Exit...\n");
			break;
		}
	}
}

int main(){
	int sockfd,connfd,len;
	struct sockaddr_in servaddr,cli;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1){
		printf("socket creation failed...\n");
		exit(0);
	}
	else{
		printf("Socket successfully created..\n");
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(PORT);
	if((bind(sockfd,(SA*)&servaddr, sizeof(servaddr)))!=0){
		printf("socket bind failed...\n");
		exit(0);
	}
	else{
		printf("Socket successfully binded..\n");
	}
	if((listen(sockfd,5))!=0){
		printf("Listen failed...\n");
		exit(0);
	}
	else{
		printf("Server listening..\n");
	}
	len=sizeof(cli);
	connfd=accept(sockfd,(SA *)&cli,&len);
	if(connfd<0){
		printf("server acccept failed...\n");
		exit(0);
	}
	else{
		printf("server acccept the client...\n");
	}
	func(connfd);
	close(sockfd);
}
