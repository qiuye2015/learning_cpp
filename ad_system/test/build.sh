input=$1
output='Test'
g++ -std=c++11 -lboost_thread -lpthread -o $output $input -g -lboost_regex
#g++ -std=c++11 -lpthread -o $output $input
#g++ -std=c++11 -lpthread -o $output $input -g -lboost_serialization-gcc48
exit
# 多文件编译
g++ -c encode_convert.cpp
g++ -c main.cpp

g++ main.o encode_convert.o -o $output
