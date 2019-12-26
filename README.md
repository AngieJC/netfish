# netfish
Netfish which is like netcat, it is my course design of software engineering.

模拟在网络工具中具有“瑞士军刀”美誉的NetCat。主要实现的功能有：端口扫描、Banner获取、传输文本与文件、远程控制、流媒体服务、远程硬盘克隆

[NetCat详解](https://blog.csdn.net/fageweiketang/article/details/82833193)

1. 端口扫描(**超时功能**)

   nf -z [ip] [ports]

2. Banner获取/客户端聊天

   nf [ip] [port]

3. 传输文本

   1. 正向：

      服务器：nf -lp [port] > [filename]

      客户端：nf [ip] [port] < [filename]

   2. 反向：

      服务器：nf -lp [port] < [filename]

      客户端：nf [ip] [port] > [filename]

4. 远程控制

   1. 正向：

      服务器：nf -lp [port] -e bash

      客户端：nf [ip] [port]

   2. 反向：

      服务器：nf -lp [port]

      客户端：nf [ip] [port] -e bash

5. 流媒体服务

   服务器：cat [filename] | nf -lp [port]

   客户端：nf [ip] [port] | mplayer -cache 3000 -

6. 远程硬盘克隆

   服务器：nf -lp [port] | dd of=[filename]

   客户端：dd if=[filename] | nf [ip] [port]

7. **ftp客户端 [测试用ftp站点：ftp.sjtu.edu.cn](ftp://ftp.sjtu.edu.cn)**

   nf -f [ip]

   1. 登录

      1. 匿名登录

      2. 匿名登录失败使用用户名密码登录

   2. 查看当前路径下所有文件

   3. 改变工作路径

   4. 下载文件

   5. 上传文件

   6. 删除文件

8. **ARP主机发现**

   nf -a [interface]
