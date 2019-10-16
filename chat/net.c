#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

void error_handling(char* message);

int main()
{
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	int clnt_addr_size;
	char message[30];
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("192.168.43.131");
	serv_addr.sin_port = htons(9190);

	clnt_addr_size = sizeof(clnt_addr);
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)	error_handling("bind() error");
	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error");
	clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1)
		error_handling("accept() error");

	while(1)
	{
		scanf("%s", message);
		if(!strcmp(message, "q"))
		{
			write(clnt_sock, message, sizeof(message));
			break;
		}
		write(clnt_sock, message, sizeof(message));
	}
	close(clnt_sock);

	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
