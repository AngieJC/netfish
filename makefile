cc=g++
opt=-lpthread -lpcap

all: nf

nf: getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.cpp ftp.o getsock.o ftpfunc.o arpfind.o arpfunc.o
	$(cc) getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.cpp ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp arpfind.cpp arp/arpfunc.cpp $(opt) -o nf

ftp.o: ftp/getsock.cpp ftp/ftpfunc.cpp ftp.cpp
	$(cc) -c ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp

arpfind.o: arpfind.cpp arp/arpfunc.cpp
	$(cc) -c arpfind.cpp arp/arpfunc.cpp $(opt)

clean:
	rm -rf nf
	rm -rf *.o
