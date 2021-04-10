#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <net/if.h>
#include <sys/ioctl.h>
using namespace std;

#define UDP 17
#define BROADCAST_IP "10.255.255.255"
#define PORT 80

int main(){
	pid_t pid;
	pid = fork();
	if(pid==0){
		int socklisten;
		if((socklisten = socket(AF_INET,SOCK_DGRAM,0))==-1){
			perror("listen socket!\n");
			exit(1);
		}
		int set = 1;
		if(setsockopt(socklisten,SOL_SOCKET,SO_REUSEADDR,&set,sizeof set) == -1){
			perror("setsockopt (SO_REUSEADDR)");
			exit(1);
		}
		struct sockaddr_in recv_addr;
		memset(&recv_addr,0,sizeof(recv_addr));
		recv_addr.sin_family = AF_INET;
		recv_addr.sin_port = htons(PORT);
		recv_addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(socklisten,(struct sockaddr *)&recv_addr,sizeof(struct sockaddr))==-1){
			perror("bind error!\n");
			exit(1);
		}
		/*Get the ip address*/
		struct ifconf ifconf;
		struct ifreq *ifreq;
		char buff[512];
		char myip[INET_ADDRSTRLEN];
		ifconf.ifc_len = 512;
		ifconf.ifc_buf = buff;
		ioctl(socklisten,SIOCGIFCONF,&ifconf);
		ifreq = (struct ifreq*) ifconf.ifc_buf;

		for (int i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--){
			if(ifreq->ifr_flags == AF_INET){
				if(strcmp(ifreq->ifr_name,"lo")!=0){
					strcpy(myip,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
					break;
				}
			}
			ifreq++;
		}

		int recvbytes;
		char recvbuff[100];
		socklen_t addrlen = sizeof(struct sockaddr_in);
		
		while(true){
			memset(&recvbuff,0,sizeof(recvbuff));
			char ip4[INET_ADDRSTRLEN];
			if((recvbytes = recvfrom(socklisten,recvbuff,100,0,(struct sockaddr*)&recv_addr,&addrlen))!=-1){
				inet_ntop(AF_INET,&(recv_addr.sin_addr),ip4,INET_ADDRSTRLEN);
				if(strcmp(myip,ip4)==0){
					continue;
				}
				cout<<recvbuff<<endl;
			}
		}
	}
	else{
		int sockfd;
		struct sockaddr_in their_addr;
		socklen_t addr_len;
		int broadcast = 1;
		if((sockfd = socket(AF_INET,SOCK_DGRAM,0))==-1){
			perror("socket!\n");
			exit(1);
		}
	
		if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof broadcast) == -1) {
	        	perror("setsockopt (SO_BROADCAST)");
	        	exit(1);
		}
	
		their_addr.sin_family = AF_INET;
		their_addr.sin_port = htons( PORT);
		inet_aton(BROADCAST_IP,&their_addr.sin_addr);
		memset(their_addr.sin_zero,0,sizeof(their_addr.sin_zero));
		cout<<"Welcome to the chatting room, input \"q\" to quit.\n";
		while(true){	
			char data[100];
			cin.get(data,100);
			cin.ignore(1024,'\n');
			if(strcmp(data,"q")==0){
				break;
			}
                        int numbytes = sendto(sockfd,data,strlen(data),0,(struct sockaddr*)&their_addr,sizeof(their_addr));

		}
		kill(pid,SIGKILL);
		wait(NULL);
		raise(SIGKILL);
	}
	return 0;
}
