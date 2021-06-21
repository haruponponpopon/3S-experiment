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

int main(int argc, char **argv){
  int s;

  int mode=0;
  
  if(argc == 2){//server
    int ss = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));
    listen(ss,10);
    struct sockaddr_in  client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    s = accept(ss, (struct sockaddr *)&client_addr, &len);
  }//server

  if(argc == 4){//server2
    mode=1;
    int ss = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(ss, (struct sockaddr *)&addr, sizeof(addr));

    listen(ss,10);
    struct sockaddr_in  client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    s = accept(ss, (struct sockaddr *)&client_addr, &len);
  }//server2

   
  else if(argc == 3){//client
    s= socket(AF_INET, SOCK_STREAM,0);
  
    struct sockaddr_in addr;
  
    addr.sin_family = AF_INET;

    if( inet_aton(argv[1], &addr.sin_addr)==0){
      perror("aton");exit(1);
    }
    addr.sin_port = htons(atoi(argv[2]));
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if(ret==-1){
      perror("connect");exit(1);
    }
  }//client

  
  FILE *gid1;
  FILE *gid2;


  int n;
  int m;
  int l;
    
  FILE *gid;
  unsigned char data;
  
  if(mode==0){
    gid = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -","r");
    FILE *wp = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w");
    while((data =  (unsigned char)fgetc(gid))!= EOF){
      m = send(s, &data, N, 0);
      if(m == -1){perror("send");exit(1);}

      unsigned char data_;
      n=recv(s,&data_,N,MSG_DONTWAIT);
      if(n!= -1){
	    l= fwrite(&data_, N, 1,wp);
	//	l=write(1,&data_,N);
		if(l==-1){perror("write");exit(1);
        }
      }
    }//whileend

    pclose(gid);
  }//ifend

    else{ char *filename ="nothere.raw";//rusuden
     unsigned char data2;
      gid1 = fopen(filename,"r");

    // gid1 = popen("play -t raw -b 16 -c 1 -e s -r 44100 nothere.raw","r");
    while((data =  (unsigned char)fgetc(gid1))!= EOF){
      m = send(s, &data, N, 0);
      if(m == -1){perror("send");exit(1);}

      unsigned char data_;
      n=recv(s,&data_,N,MSG_DONTWAIT);
      if(n!= -1){
	l=write(1,&data_,N);

	//if(l==-1){perror("write");exit(1);
    }
      
        
    }//whileend
 
    pclose(gid1);
    //  pclose(gid2);
  }
  close(s);
}
