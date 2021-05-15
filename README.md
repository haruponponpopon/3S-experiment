# 3S-experiment
三年生の前期実験で行った発展課題のソースコードを載せていきます。  
### doremi.c  
ファイルをコンパイルします。  
```bash  
clang doremi.c -o doremi  
```  
soxコマンドを使えるようにした後、以下のコマンドを実行すると振幅10000、5音階分の音階が再生されます。  
```bash
./doremi 10000 5 | play -t raw -b 16 -c 1 -e s -r 44100 -
```  
Mid1Fから始まり、次の音に移る時前の音の周波数に2^xかけることになりますが、array配列の中に、1オクターブ文のxを入れておき、半音上がる時、全音上がる時の区別ができるようにした。  
