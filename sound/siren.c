#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
//何周期表示したいか
int main(int argc, char ** argv){
    double A = atof(argv[1]);
    double A1 = A;
    int n = atoi(argv[2]);
    double sample = 13230;
    int num = 0;
    double ra[2] = {880*pow(2,1.0/6.0), 880/pow(2, 1.0/6.0)};

    while(num<n*2){
        for (int i=0; i<13230*2; i++){
            A = A*sin((double)(num*13230*2+i)/(13230*2*2*n)*M_PI);
            short p = A*sin(2* M_PI * ra[num%2] * i/44100);
            write(1,&p,2);
            A = A1;
        }
        A = A1;
        num++;
    }
    
    return 0;
}
