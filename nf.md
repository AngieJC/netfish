模拟在网络工具中具有“瑞士军刀”美誉的NetCat。主要实现的功能有：端口扫描、Banner获取、传输文本与文件、远程控制、流媒体服务、远程硬盘克隆

[NetCat详解](https://blog.csdn.net/fageweiketang/article/details/82833193)

1. 端口扫描

   nf -z [ip] [ports]

2. Banner获取

   nf [ip] [port]

3. 传输文本

   1. 正向：

      服务器：nf -lp [port] > [filename]

      客户端：nf [ip] [port] < filename

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