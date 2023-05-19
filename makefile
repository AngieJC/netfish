cc=g++
opt=-lpthread -lpcap
optimization=-o3 -funroll-loops

all: nf

nf: getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.o icmpfunc.o ftp.o getsock.o ftpfunc.o arpfind.o arpfunc.o
	$(cc) getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp scan.cpp exec.cpp getip.cpp traceroute.cpp icmp/icmpfunc.cpp ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp arpfind.cpp arp/arpfunc.cpp $(opt) $(optimization) -o nf

ftp.o: ftp/getsock.cpp ftp/ftpfunc.cpp ftp.cpp
	$(cc) -c ftp.cpp ftp/getsock.cpp ftp/ftpfunc.cpp

arpfind.o: arpfind.cpp arp/arpfunc.cpp
	$(cc) -c arpfind.cpp arp/arpfunc.cpp $(opt)

traceroute.o: traceroute.cpp icmp/icmpfunc.cpp
	$(cc) -c traceroute.cpp icmp/icmpfunc.cpp $(opt)

icmpfunc.o: icmp/icmpfunc.cpp
	$(cc) -c icmp/icmpfunc.cpp

clean:
	rm -rf *.o

distclean:
	rm -rf nf
