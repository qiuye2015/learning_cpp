# 多文件编译
g++ -c encode_convert.cpp
g++ -c main.cpp

g++ main.o encode_convert.o -o test
rm main.o encode_convert.o
