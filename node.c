#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PORT 49491
#define SA struct sockaddr
#define MAX 100

void *fungsi (int socketfd){
	char buff[MAX];
	char dariClient[MAX];
	bzero(buff, MAX);
	strcpy(buff, "#munin node at MyComputer\n");
	write(socketfd, buff, sizeof(buff));
	for(;;){
		bzero(buff, MAX);
		read(socketfd, buff, sizeof(buff));
		strcpy(dariClient, buff);
		printf("Dari client: %s", dariClient);
		bzero(buff, MAX);
		if(strncmp("cap", dariClient, 3) == 0){		
			strcpy(buff, "cap multigraph dirtyconfig\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if (strncmp("nodes", dariClient, 5) == 0){
			strcpy(buff, "MyComputer\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, ".\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if(strncmp("list MyComputer", dariClient, 15) == 0){
			strcpy(buff, "memory\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if(strncmp("config memory", dariClient, 13) == 0){
			strcpy(buff, "graph_args --base 1024 -l 0 --upper-limit 8271892480\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "graph_vlabel Bytes\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "graph_title Memory Usage\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "graph_category system\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "graph_info This graph shows this machine memory\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "graph_order used free\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "used.label used\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "used.draw STACK\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "used.info Used memory\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "free.label free\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "free.draw STACK\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, "free.info Free memory\n");
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, ".\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if(strncmp("fetch memory", dariClient, 12) == 0){
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
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
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
			write(socketfd, buff, sizeof(buff));
			bzero(buff, MAX);
			strcpy(buff, ".\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if(strncmp("version", dariClient, 7) == 0){
			strcpy(buff, "lovely node on MyComputer version: 8.48\n");
			write(socketfd, buff, sizeof(buff));
		}
		else if(strncmp("quit", dariClient, 4) == 0){
			close(socketfd);
			break;
		}
		else{
			strcpy(buff, "# Unknown command. Try cap, list, nodes, config, fetch, version or quit\n");
			write(socketfd, buff, sizeof(buff));
		}	
	}
}

int main(){
	int socketfd, connectfd, len;
	struct sockaddr_in serveraddr, client;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd == -1){
		printf("pembuatan socket gagal\n");
		exit(0);
	}
	else{
		printf("socket berhasil dibuat\n");
	}
	bzero(&serveraddr, sizeof(&serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	if((bind(socketfd, (SA*)&serveraddr, sizeof(serveraddr))) != 0){
		printf("socket bind gagal\n");
		exit(0);
	}
	else{
		printf("socket bind berhasil\n");
	}
	if(listen(socketfd, 5) != 0){
		printf("gagal mendengar\n");
		exit(0);
	}
	else{
		printf("server mendengar\n");
	}
	len = sizeof(client);
	pthread_t tid;
	int i;
	for (i = 0; i < 3; i++) {
		connectfd = accept(socketfd, (SA*)&client, &len);
		if(connectfd >= 0){
			pthread_create(&tid, NULL, fungsi, (void *)connectfd);
		}
	}
	close(socketfd);
	
	return 0;
}
