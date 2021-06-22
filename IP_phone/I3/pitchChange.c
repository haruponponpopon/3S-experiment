#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <complex.h>
#include <math.h>
#include <string.h>

typedef short sample_t;

void die(char * s) {
  perror(s); 
  exit(1);
}

ssize_t read_n(int fd, ssize_t n, void * buf) {
  ssize_t re = 0;
  while (re < n) {
    ssize_t r = read(fd, buf + re, n - re);
    if (r == -1) die("read");
    if (r == 0) break;
    re += r;
  }
  memset(buf + re, 0, n - re);
  return re;
}

ssize_t write_n(int fd, ssize_t n, void * buf) {
  ssize_t wr = 0;
  while (wr < n) {
    ssize_t w = write(fd, buf + wr, n - wr);
    if (w == -1) die("write");
    wr += w;
  }
  return wr;
}

void sample_to_complex(sample_t * s,  complex double * X, long n) {
  long i;
  for (i = 0; i < n; i++) X[i] = s[i];
}

void complex_to_sample(complex double * X, 
		       sample_t * s, 
		       long n) {
  long i;
  for (i = 0; i < n; i++) {
    s[i] = creal(X[i]);
  }
}

void fft_r(complex double * x, 
	   complex double * y, 
	   long n, 
	   complex double w) {
  if (n == 1) { y[0] = x[0]; }
  else {
    complex double W = 1.0; 
    long i;
    for (i = 0; i < n/2; i++) {
      y[i]     =     (x[i] + x[i+n/2]);
      y[i+n/2] = W * (x[i] - x[i+n/2]);
      W *= w;
    }
    fft_r(y,     x,     n/2, w * w);
    fft_r(y+n/2, x+n/2, n/2, w * w);
    for (i = 0; i < n/2; i++) {
      y[2*i]   = x[i];
      y[2*i+1] = x[i+n/2];
    }
  }
}

void fft(complex double * x, 
	 complex double * y, 
	 long n) {
  long i;
  double arg = 2.0 * M_PI / n;
  complex double w = cos(arg) - 1.0j * sin(arg);
  fft_r(x, y, n, w);
  for (i = 0; i < n; i++) y[i] /= n;
}

void ifft(complex double * y, complex double * x, long n) {
  double arg = 2.0 * M_PI / n;
  complex double w = cos(arg) + 1.0j * sin(arg);
  fft_r(y, x, n, w);
}

void bandpass(complex double * y, long min, long max, long n, long fs){
  for(int i=0; i<n; i++){
    if(i*fs/n < min || i*fs/n > max){y[i]=0;}
    else{y[i]*=5;}
  }
}

void bandpass2(complex double * y, long min, long max, long n, long fs){
  for(int i=0; i<n/2; i++){
    if(i*fs/n < min || i*fs/n > max){
        y[i]=0;
        y[n-i-1]=0;
    }
    else{
        y[i]*=5;
        y[n-i-1]*= 5;
    }
  }
}

void bandpass3(complex double * y, long min, long max, long n, long fs){
  for(int i=0; i<n; i++){
    if(i*fs/n < min || i*fs/n > max){
        y[i]=0;
    }
    else{
        y[i]*=5;
    }
  }
  for (int i=0; i<n; i++){
      if (i*fs/n >= min && i*fs/n <= max){
          if (i-3<n&&i-3>=0) {
            y[i-3]=y[i];
          }
      }
  }
}


void pitch(complex double * y, long n, double v){
  complex double tmp[n];
  for(long i=0; i<n; ++i){
    tmp[i] = y[i];
    y[i] = 0;
  }
  long i = 0;
  while(i*v<n){
    long j = (long)i*v;
    y[j] = tmp[i];
    ++i;
  }
}


int pow2check(long N) {
  long n = N;
  while (n > 1) {
    if (n % 2) return 0;
    n = n / 2;
  }
  return 1;
}

int main(int argc, char** argv){
  long n = 1024;
  sample_t * buf = calloc(sizeof(sample_t), n);
  complex double * X = calloc(sizeof(complex double), n);
  complex double * Y = calloc(sizeof(complex double), n);
  if(argc == 2){
    int ss = socket(PF_INET, SOCK_STREAM, 0);
    if(ss == -1){perror("socket");exit(1);}
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = INADDR_ANY;
    int b = bind(ss, (struct sockaddr *)&addr, sizeof(addr));
    if(b == -1){perror("bind");exit(1);}
    int l = listen(ss, 10);
    if(l == -1){perror("listen");exit(1);}

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int s = accept(ss, (struct sockaddr *)&client_addr, &len);
    if(s == -1){perror("accept");exit(1);}
    close(ss);
    short data[n];
    FILE *fp = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r");
    if(fp == NULL){perror("popen");exit(1);}
    FILE *wp = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w");
    if(wp == NULL){perror("popen");exit(1);}
    while(1){
      int r = fread(buf, 2, n, fp);
      if(r == 0){break;}
      sample_to_complex(buf, X, n);
      fft(X, Y, n);
      bandpass3(Y, 10, 10000, n, 44100);
      // pitch(Y, n, 1.5);
    //   pitch(Y,n,1.1);
      ifft(Y, X, n);
      complex_to_sample(X, buf, n);
      int m = send(s, buf, 2*r, 0);
      if(m == -1){perror("send");exit(1);}
      
      int a = recv(s, data, n*2, 0);
      if(a == -1){perror("recv");exit(1);}
      if(a == 0){break;}
      fwrite(data, 2, a/2, wp);
    }
    pclose(fp);
    pclose(wp);
    close(s);
  }
  if(argc==3){
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if(s == -1){perror("socket");exit(1);}
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    int tmp = inet_aton(argv[1], &addr.sin_addr);
    if (tmp == 0){perror("inet_aton");exit(1);}
    addr.sin_port = htons(atoi(argv[2]));
    int ret = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1){perror("connect");exit(1);}
    short data[n];
    FILE *fp = popen("rec -t raw -b 16 -c 1 -e s -r 44100 -", "r");
    if(fp == NULL){perror("popen");exit(1);}
    FILE *wp = popen("play -t raw -b 16 -c 1 -e s -r 44100 -", "w");
    if(wp == NULL){perror("popen");exit(1);}
    while(1){
      int a = recv(s, data, n*2, 0);
      if(a == -1){perror("recv");exit(1);}
      if(a == 0){break;}
      fwrite(data, 2, a/2, wp);
      
      int r = fread(buf, 2, n, fp);
      if(r == 0){break;}
      sample_to_complex(buf, X, n);
      fft(X, Y, n);
      bandpass3(Y, 10, 10000, n, 44100);
      // pitch(Y, n, 1.5);
    //   pitch(Y,n,1.1);
      ifft(Y, X, n);
      complex_to_sample(X, buf, n);
      int m = send(s, buf, 2*r, 0);
      if(m == -1){perror("send");exit(1);}
    }
    pclose(fp);
    pclose(wp);
    close(s);
  }
}
