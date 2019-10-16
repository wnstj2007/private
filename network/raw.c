#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>

int main()
{
	unsigned char packet[40];
	int raw_socket;
	int on = 1;
	struct iphdr *iphdr;
	struct tcphdr *tcphdr;
	struct sockaddr_in address;
	
	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, (char*)&on, sizeof(on));

	tcphdr = (struct tcphdr*)(packet+20);
	memset((char*)tcphdr, 0, 20);
	tcphdr -> source = htons(777);
	tcphdr -> dest = htons(12345);
	tcphdr -> seq = htonl(92929292);
	tcphdr -> ack_seq = htonl(12121212);
	tcphdr -> doff = 5;
	tcphdr -> syn = 1;
	tcphdr -> window = htons(512);
	tcphdr -> check = 1;

	iphdr = (struct iphdr*)packet;
	memset((char*)iphdr, 0, 20);
	iphdr -> version = 4;
	iphdr -> ihl = 5;
	iphdr -> protocol = IPPROTO_TCP;
	iphdr -> tot_len = 40;
	iphdr -> id = htons(777);
	iphdr -> ttl = 120;
	iphdr -> check = 1;
	iphdr -> saddr = inet_addr("111.111.111.111");
	iphdr -> daddr = inet_addr("192.168.0.8");

	address.sin_family = AF_INET;
	address.sin_port = htons(12345);
	address.sin_addr.s_addr = inet_addr("192.168.0.8");

	sendto(raw_socket, &packet, sizeof(packet), 0x0, (struct sockaddr*)&address, sizeof(address));
}
