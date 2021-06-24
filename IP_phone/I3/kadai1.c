#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#define N 1

void die(char * s) {
    perror(s); 
    exit(1);
}

int main(int argc, char **argv){
    if (argc!=2 && argc!=3) die("invalid　stdin");
    int a,s;
    unsigned char data;

    /*サーバー側*/
    if(argc == 2){
        int ss = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(argv[1]));
        addr.sin_addr.s_addr = INADDR_ANY;
        a = bind(ss, (struct sockaddr *)&addr, sizeof(addr));
        if (a<0) die("reader: bind");
        a = listen(ss,10);
        if (a<0) die("reader: bind");
        struct sockaddr_in  client_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        s = accept(ss, (struct sockaddr *)&client_addr, &len);
        if (s<0) die("reader: accept");
    }

    /*クライアント側*/
    else if(argc == 3){
        s = socket(AF_INET, SOCK_STREAM,0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        a = inet_aton(argv[1], &addr.sin_addr);
        if (a==0)die("inet_aton");
        addr.sin_port = htons(atoi(argv[2]));
        a = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        if  (a<0) die("connect");
    }

    int n;
    int l;
    FILE *pp;
    pp = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -","r");
    FILE *wp = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w");
    
    while((data =  (unsigned char)fgetc(pp))!= EOF){
        a = send(s, &data, N, 0);
        if (a<0) die("send");
        unsigned char data_;
        a=recv(s,&data_,N,MSG_DONTWAIT);
        if(a!= -1){
            l= fwrite(&data_, N, 1,wp);
            if (l<0) die("write");
        }
    }

    pclose(pp); 
 
    close(s);
}
