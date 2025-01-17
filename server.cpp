#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<string.h>
#include <iostream>
#include <thread>
using std::thread;
using std::cout;
using std::cin;
using std::endl;

#define MAX_ONLINE 10

char g_buf[MAX_ONLINE][1024];
int g_online_total;

struct User {
        int user_sock;
        struct sockaddr_in user_addr;
        int id;
        bool online;
};
struct User g_users[MAX_ONLINE];


void doHandler(int client_sock, struct sockaddr_in client_addr, int id) {
        char buf[1024];
        int n;
        char saddr[1024];
        int len;
        g_users[id].user_sock = client_sock;
        g_users[id].id = id;
        g_users[id].online = true;
        g_users[id].user_addr = client_addr;
        g_online_total += 1;

        memset(buf, 0, 1024);
        len = sprintf(buf, "client%d online:-IP:%s   -port:%d\nonline user=%d",
                id,
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, saddr, 1024),
                ntohs(client_addr.sin_port),
                g_online_total);
        cout << buf << endl;
        for (int i = 0;i < MAX_ONLINE;i++) {
                if (id == i) {
                        char temp_buf[128];
                        int temp_len = sprintf(temp_buf, "Welcome to chatroom! your id is:%d\n", id);
                        send(client_sock, temp_buf, temp_len, 0);
                }
                if (g_users[i].online) {
                        send(g_users[i].user_sock, buf, len, 0);
                }
        }
        memset(buf, 0, 1024);
        fflush(stdout);
        fflush(stdin);
        while ((n = read(client_sock, buf, 1024))) {

                printf("client%d:%s\n", id, buf);
                if (0 == strcmp(buf, "exit")) {
                        break;
                }
                memset(g_buf[id], 0, 1024);
                //len = sprintf(g_buf[id], "client%d:", id);
		len=0;
                memcpy(g_buf[id] + len, buf, 1024 - len);
                memset(buf, 0, 1024);

                for (int i = 0;i < MAX_ONLINE;i++) {
                        if (g_users[i].online && i!=id) {
                                send(g_users[i].user_sock, g_buf[id], strlen(g_buf[id]), 0);
                        }
                }
        }
        memset(buf, 0, 1024);
        len = sprintf(buf, "client%d offline. online user:%d\n", id, g_online_total - 1);
        cout << buf << endl;
        for (int i = 0;i < MAX_ONLINE;i++) {
                if (i == id) {
                        continue;
			char temp_buf[128];
                        int temp_len = sprintf(temp_buf, "Goodbye");
                        send(client_sock, temp_buf, temp_len, 0);
                }
                if (g_users[i].online) {
                        send(g_users[i].user_sock, buf, len, 0);
                }
        }
        g_users[id].online = false;
        g_online_total -= 1;
        close(client_sock);
}


int main() {
        int server_sock = socket(AF_INET, SOCK_STREAM, 0);
        int client_total = 0;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8888);
        unsigned int dst;
        inet_pton(AF_INET, "127.0.0.1", &dst);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        int ret = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));

        if (ret == -1) {
                perror("bind error");
                exit(1);
        }
        ret = listen(server_sock, MAX_ONLINE);//Max connections
        if (ret == -1) {

                perror("listen error");
                exit(1);
        }

        //current online users number
        g_online_total = 0;
	for(int i=0;i<MAX_ONLINE;i++){
	    g_users[i].online=false;
	}


        while (true) {
                int comfd;
                struct sockaddr_in comaddr;
                socklen_t addrlen = sizeof(sockaddr_in);
                if (comfd = accept(server_sock, (struct sockaddr*)&comaddr, &addrlen)) {
			int id=0;
			//找到第一个可用的ID
                        for(int i=0;i<MAX_ONLINE;i++){
			    if(!g_users[i].online){
				id=i;
		 		break;
			    }
			}
			thread t(doHandler, comfd, comaddr, id);
                        t.detach();

                }
        }
        close(server_sock);
}
