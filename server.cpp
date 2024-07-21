#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<string.h>

int main(){
        int fd=socket(AF_INET,SOCK_STREAM,0);
        if(fd==-1){
                perror("socket error");
                exit(1);
        }
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        addr.sin_port=htons(8888);
        unsigned int dst;
        inet_pton(AF_INET,"127.0.0.1",&dst);
        addr.sin_addr.s_addr=htonl(INADDR_ANY);
        int ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr));
        if(ret==-1){
                perror("bind error");
                exit(1);
        }
        ret=listen(fd,5);
        if(ret==-1){

                 perror("listen error");
                exit(1);
        }
        struct sockaddr_in comaddr;
        socklen_t addrlen=sizeof(comaddr);
        int comfd=accept(fd,(struct sockaddr*)&comaddr,&addrlen);
        if(comfd==-1){
                perror("accept error");
                exit(1);
        }
        printf("accept\n");
        char buf[1024];
        int n;
        char saddr[1024];
        printf("client connected:-IP:%s   -port:%d\n",
        inet_ntop(AF_INET,&comaddr.sin_addr.s_addr,saddr,1024),
                ntohs(comaddr.sin_port));
	fflush(stdout);
	fflush(stdin);
        while((n=read(comfd,buf,1024))){
                if(n==-1){
                        perror("read error");
                        exit(1);
                }
		/*
                for(int i=0;i<n;i++){
                        buf[i]=toupper(buf[i]);
                }
		*/
		//ret=write(comfd,buf,n);
                printf("get message:%s\n",buf);
		if(0==strcmp(buf,"exit")){
			break;		
		}
		if(ret==-1){
                        perror("server write error");
                        exit(1);
                }
		memset(buf,0,1024);
        }
	close(fd);



}
