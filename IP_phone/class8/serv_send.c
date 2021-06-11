#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
int main(int argc, char ** argv){
    int nr, nw;
    if (argc!=2){
        perror("Invalid");
        exit(0);
    }
    int N = 1;
    unsigned char data[N];
    int ss;
    int a;
    /* ソケットの生成 */
    if ((ss = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("reader: socket");
        exit(1);
    }
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    a = bind(ss, (struct sockaddr *) &addr, sizeof(addr));
    if (a<0){
        perror("reader: bind");
        exit(1);
    }
    a = listen(ss,10);
    if (a<0){
        perror("reader: listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int s = accept(ss, (struct sockaddr *)&client_addr, &len);
    if (s<0){
        perror("reader: accept");
    }
    close(ss);
    while(1){
        nr = read(0,data,N);
        if (nr<0){
            perror("reader: read");
            exit(1);
        }
        if (nr==0) break;
        nw = send(s,data,nr, 0);
        if (nw<0){
            perror("reader: write");
            exit(1);
        }
    }
    close(s);
    return 0;
}
