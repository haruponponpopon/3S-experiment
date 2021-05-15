#include <stdio.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
//何オクターブ表示したいか
int main(int argc, char ** argv){
    int A = atoi(argv[1]);
    int n = atoi(argv[2]);
    int sample = 13230;
    int num = 0;
    double array[7] = {1.0/6.0,1.0/6.0,1.0/12.0,1.0/6.0,1.0/6.0,1.0/6.0,1.0/12.0};
    int current = 5;  //ラなので5番目から開始
    double ra = 220;

    while(num<n*7){
        for (int i=0; i<13230; i++){
            short p = A*sin(2* M_PI * ra * i/44100);
            write(1,&p,2);
        }
        ra *= pow(2,array[current%7]);
        current+=1;
        num++;
    }
    
    return 0;
}
