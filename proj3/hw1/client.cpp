#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
using namespace std;
#define TCP 6
#define UDP 17
#define SERVER_IP "10.0.0.1"
#define SERVER_PORT "80"
/*
struct addrinfo {
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname

        struct addrinfo *ai_next;      // linked list, next node
    };

struct sockaddr {
        unsigned short    sa_family;    // address family, AF_xxx
        char              sa_data[14];  // 14 bytes of protocol address
};
*/

int main(){
	struct addrinfo hints, *servinfo,*p;
	int sockfd;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	int rv = getaddrinfo(SERVER_IP,SERVER_PORT,&hints,&servinfo);
	for(p=servinfo;p!=NULL;p=p->ai_next){
		sockfd = socket(p->ai_family,p->ai_socktype,TCP);
		if(sockfd==-1){
			cout<<"creating socket...\n";
			continue;
		}
		int flag = connect(sockfd,p->ai_addr,p->ai_addrlen);
		if(flag == -1){
			cout<<"connecting to server...\n";
			continue;
		}
		break;
	}
	cout<<sockfd<<endl;
	if(p==NULL){
		cout<<"fail to connect!\n";
	}
	pid_t pid;
	pid = fork();
	if(pid<0){cout<<"fork error!\n";}
	else if(pid == 0){
		while(true){
			char buff[100];
			int bytes_recieve;
			memset(buff,0,sizeof(buff));
			bytes_recieve = recv(sockfd,buff,100,0);
			if(bytes_recieve>0){cout<<buff<<endl;}
		}	
	}
	else{
		sleep(2);
		while(true){
			char send_info[100];
			cin.get(send_info,100);
			cin.ignore(1024,'\n');
			int bytes_send = send(sockfd,send_info,strlen(send_info),0);
			if(strcmp(send_info,"q")==0){
				break;	
			}
		}
		kill(pid,SIGKILL);
		wait(NULL);
		raise(SIGKILL);
	}
	return 0;
}
