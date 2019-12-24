cc=g++
opt=-lpthread

all: nf

nf: getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.cpp ftp.o getsock.o ftpfunc.o
	$(cc) getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.cpp ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp $(opt) -o nf

ftp.o: ftp/getsock.cpp ftp/ftpfunc.cpp ftp.cpp
	$(cc) -c ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp

clean:
	rm -rf nf
	rm -rf *.o
