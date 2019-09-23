#!/bin/bash
BOOST_HOME=/home/work/fjp/lib/boost_1_69_0_bin
if [ "$#" == "2" ];then
input=$1
output=$2
g++ -std=c++11 -I $BOOST_HOME/include -L $BOOST_HOME/lib -lboost_thread -lpthread -lboost_serialization -lboost_program_options -g -o $output $input
#gcc -std=c++11 -I $BOOST_HOME/include -L $BOOST_HOME/lib -lboost_thread -lboost_serialization -lboost_program_options -g -o $output $input
echo "success!!"
fi
