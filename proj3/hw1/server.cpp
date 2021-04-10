#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;
pthread_mutex_t mutex;
vector<int> chatting_room;
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

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void *client_fun(void *arg)
{
	int connfd = *((int*)arg);
	cout<<"created a thread to serve "<<connfd<<endl;
	while(true){
		int recv_len = 0;
		char recv_buff[100];
		memset(recv_buff,0,sizeof(recv_buff));
		recv_len = recv(connfd,recv_buff,100,0);
		cout<<recv_len<<endl;
		if(recv_len>0&&strcmp(recv_buff,"q")!=0){
			pthread_mutex_lock(&mutex);
			cout<<"server recieved "<<recv_buff<<" from "<<connfd<<endl;
			for(int i=0;i<chatting_room.size();++i){
				cout<<chatting_room[i]<<" ";
			}cout<<"\n";
			/*pthread_mutex_lock(&mutex);*/
			for(int i=0;i<chatting_room.size();++i){
				if(chatting_room[i]!=connfd){
					send(chatting_room[i],recv_buff,strlen(recv_buff),0);
				}
			}
			pthread_mutex_unlock(&mutex);
		}
		else if(recv_len>0&&strcmp(recv_buff,"q")==0){
			pthread_mutex_lock(&mutex);
			for(int i=0;i<chatting_room.size();++i){
				if(chatting_room[i]==connfd){
					for(int j=i;j<chatting_room.size()-1;++j){
						chatting_room[j] = chatting_room[j+1];
					}
				break;
				}
				
			}
			chatting_room.pop_back();
			pthread_mutex_unlock(&mutex);
			int close_res = close(connfd);
			if(close_res==0){cout<<"close socket "<<connfd<<" successfully!\n";}
			else{cout<<"cannot close socket "<<connfd<<"\n";}
			for(int i=0;i<chatting_room.size();++i){cout<<chatting_room[i]<<" ";}cout<<"\n";
			/*pthread_mutex_unlock(&mutex);*/
			break;
		}
		else{
			break;
		}
	}
	cout<<"new thread killed\n";
	return NULL;
}





#define TCP 6
#define UDP 17
#define BACKLOG 20


int main(int argc, char *argv[])
{
	struct addrinfo hints, *servinfo, *p;
	int sockfd;
	pthread_t thread_id;
	

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv = getaddrinfo(NULL,"80",&hints,&servinfo);
	for(p=servinfo;p!=NULL;p=p->ai_next){
		sockfd = socket(p->ai_family,p->ai_socktype,TCP);
		if(sockfd==-1){
			cout<<"creating socket..."<<endl;
			continue;
		}
		int flag = bind(sockfd,p->ai_addr,p->ai_addrlen);
		if(flag == -1){
			cout<<"binding to this socket..."<<endl;
		}
		break;
	}
	cout<<sockfd<<endl;
	freeaddrinfo(servinfo);
	if(p==NULL){
		cout<<"fail to bind!"<<endl;
		exit(1);
	}
	if(listen(sockfd,BACKLOG)==-1){
		cout<<"listen error!\n";
		exit(1);
	}
	while(true){
		struct sockaddr_storage their_addr;
		socklen_t addr_size;
		addr_size = sizeof their_addr;
		int newfd = accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
		
		/*Send the welcome message*/
		char msg[100] = "Welcome to login the chatting room!,send \"q\" to quit.";
		int len,bytes_sent;
		len = strlen(msg);
		bytes_sent = send(newfd,msg,len,0);
		cout<<"sended "<<bytes_sent<<" data\n";
		/*Show the login information*/
		char s[INET6_ADDRSTRLEN];
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof s);
		cout<<"accepted a connection ! via "<<s<<" with the newfd "<<newfd<<"\n";
		

		pthread_mutex_lock(&mutex);
		chatting_room.push_back(newfd);
		pthread_mutex_unlock(&mutex);
		
		pthread_create(&thread_id,NULL,client_fun,(void*)(&newfd));
		pthread_detach(thread_id);

	}
	return 0;
}
