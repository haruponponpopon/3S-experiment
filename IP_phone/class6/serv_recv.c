#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char ** argv){
    if (argc!=3){ perror("argc"); exit(1); }
    unsigned char data;
    int s = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret==-1){ perror("connect"); exit(1); }



    int n;
    while(1){
        n = read(s, &data, 1);
        if (n==-1){ perror("read"); exit(1); }
        if (n==0) break;
        n = write(1, &data, 1);
        if (n==-1){ perror("write"); exit(1); }
        if (n==0) break;
    }
    close(s);
    // shutdown(s, SHUT_WR);
}
