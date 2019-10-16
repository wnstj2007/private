#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#define BUF 100
#define NOM 20

void *send_msg(void* arg);
void *recv_msg(void* arg);
void error_handling(char* msg);

void menu();
void changeName();
void menuOption();

char name[NOM] = "[DEFAULT]";
char msg_from[NOM];
char serv_time[NOM];
char msg[BUF];
char serv_port[NOM];
char clnt_ip[NOM];

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t send_thread, recv_thread;
	void* thread_return;

	if(argc != 5)
	{
		printf("Usage : %s <ip> <port> <name> <server>\n", argv[0]);
		exit(1);
	}

 	//local time
	struct tm *t;
	time_t timer = time(NULL);
	t = localtime(&timer);
	sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, 
	t->tm_mday, t->tm_hour, t->tm_min);
	
	sprintf(name, "[%s]", argv[3]);
	sprintf(clnt_ip, "%s", argv[1]);
	sprintf(serv_port, "%s", argv[2]);
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[4]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	//call menu
	menu();

	pthread_create(&send_thread, NULL, send_msg, (void*)&sock);
	pthread_create(&recv_thread, NULL, recv_msg, (void*)&sock);
	pthread_join(send_thread, &thread_return);
	pthread_join(recv_thread, &thread_return);
	
	close(sock);
	return 0;
}

void *send_msg(void *arg)
{
	int sock = *((int*)arg);
	char name_msg[NOM+BUF];
	char myInfo[BUF];
	char *who = NULL;
	char temp[BUF];

	//send join message
	printf(" >> join the chat !! \n");
	sprintf(myInfo, "%s's join. IP_%s\n", name, clnt_ip);
	write(sock, myInfo, strlen(myInfo));

	while(1)
	{
		fgets(msg, BUF, stdin);

		//menu_mode command -> !menu
		if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}

		//send message
		sprintf(name_msg, "%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));
	}

	return NULL;
}

void *recv_msg(void *arg)
{
	int sock = *((int*)arg);
	char name_msg[NOM+BUF];
	int str_len;

	while(1)
	{
		str_len = read(sock, name_msg, NOM+BUF-1);
		if(str_len == -1)
			return (void*)-1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

void menuOption()
{
	int select;

	printf("\n\t**** menu mode ****");	
	printf("\t1. change name\n");
	printf("\t2. clear/update\n\n");
	printf("\tthe other key is cancel");
	printf("\n\t*******************");
	printf("\n\t>> ");
	scanf("%d", &select);
	getchar();
	switch(select)
	{
		case 1:
			changeName;
			break;

		//console update(time, clear chatting log)
		case 2:
			menu();
			break;

		//menu error
		default:
			printf("\tcancel.");
			break;
	}
}

void changeName()
{
	char newName[100];
	printf("\n\tInput new name -> ");
	scanf("%s", newName);
	sprintf(name, "[%s]", newName);
	printf("\n\tComplete.\n\n");
}

void menu()
{
	system("clear");
	printf("**** ARGOS network team chatting client ****\n");
	printf("server port : %s\n", serv_port);
	printf("client IP   : %s\n", clnt_ip);
	printf("chat name   : %s\n", name);
	printf("server time : %s\n", serv_time);
	printf("****************** menu ********************\n");
	printf("if you want to select to menu -> !menu\n");
	printf("1. change name\n");
	printf("2. clear/update\n");
	printf("********************************************\n");
	printf("Exit -> q & Q\n\n");
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
