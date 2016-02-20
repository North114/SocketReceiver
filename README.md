##Socket数据接收程序

###新手入门
1. 安装Linux系统，打开**terminal**，输入 
```
ssh root@121.43.109.2
``` 
登录阿里云服务器，进入之后，在服务器的**/usr/share/nginx/html/**文件夹下面存放的是
服务器端的网页程序和Socket程序，网页程序包括了html、css、javascript和php，其中，。

###如何使用
1. 运行[shell文件](./Compile.sh)或者[MAKEFILE](./Makefile)
2. 运行生成的可执行文件，例如$./output &

##主程序
1. [接收程序](./socket_receiver_v02.c)
2. 主程序所在文件夹**/home/webAdmin/SocketReceiver/newVersion**

##其他
* 如果你想将服务器上的文件拷贝到自己电脑，可以使用如下命令，前提还是**Linux**系统
```
scp -r root@121.43.109.2:path-of-file-you-want-to-copy local-path-you-want-to-store-that-file
```
