cc=g++
opt=-lpthread

all: nf

nf: netfish.cpp getopt.cpp
	$(cc) getopt.cpp netfish.cpp listen.cpp help.cpp connect.cpp $(opt) -o nf

clean:
	rm -rf nf
