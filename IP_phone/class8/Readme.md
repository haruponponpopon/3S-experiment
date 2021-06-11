# serv_send.c  
片道電話  
サーバーから入力した音をクライアント側で鳴らせる。  
#### サーバー側
```
$gcc serv_send.c -o serv_send  
$rec -t raw -b 16 -c 1 -e s -r 44100 - | ./serv_send ポート番号
```
#### クライアント側(class6のclient_recv.c参照)  
```
$gcc client_recv.c -o client_recv  
$./client_recv IPアドレス ポート番号 | play -t raw -b 16 -c 1 -e s -r 44100 -  
```
