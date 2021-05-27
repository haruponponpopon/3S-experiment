#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>


#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>



int main(int argc, char ** argv){

    int sockfd;
    // int writer_len;

    struct sockaddr_in reader_addr; 

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("reader: socket");
        exit(1);
    }

    

    reader_addr.sin_family = AF_INET;
    reader_addr.sin_addr.s_addr = inet_addr(argv[1]);
    reader_addr.sin_port = htons(atoi(argv[2]));



    int pq = sendto(sockfd, "a", 1, 0, (struct sockaddr *)&reader_addr, sizeof(reader_addr));
    if (pq==-1){
        perror("reader: send to");
        exit(1);
    }

    int N = 1000; //1000バイト読み込む
    while(1){
        unsigned char data[N];
        int n=recvfrom(sockfd, data, sizeof(data), 0, NULL, 0);
        if (n==-1){
            perror("recv");
            exit(1);
        } else if (n==0){
            fprintf(stderr, "EOF!!\n");
            break;
        }
        bool flag = false;
        if (n==N){
            int i;
            for (i=0; i<N; i++){
                if (data[i]!='1') break;
                if (i==N-1) flag = true;
            }

        }
        if (flag) break;

        n = write(1, data, n);
        if (n==-1){ perror("write"); exit(1); }
        if (n==0) break;
    }
    close(sockfd); 
    return 0;

}



