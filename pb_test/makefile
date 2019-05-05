# INC=/usr/local/include
INC=/home/work/fjp/test/cpp/pb_test -I .
LIB=/usr/local/lib

lib=protobuf

all:Writer Reader

Writer.o:Writer.cpp
	g++ -g -c Writer.cpp -I$(INC) -L$(LIB) -l$(lib)
Reader.o:Reader.cpp
	g++ -g -c Reader.cpp -I$(INC) -L$(LIB) -l$(lib)
Writer:Writer.o Mymessage.pb.o
	g++ -g -o Writer Writer.o Mymessage.pb.o -I$(INC) -L$(LIB) -l$(lib)
Reader:Reader.o Mymessage.pb.o
	g++ -g -o Reader Reader.o Mymessage.pb.o -I$(INC) -L$(LIB) -l$(lib)
Mymessage.pb.o:Mymessage.pb.cc
	g++ -g -c Mymessage.pb.cc -I$(INC) -L$(LIB) -l$(lib)
