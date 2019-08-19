BOOST_HOME_2=/home/fjp/bin/boost_1_69_bin
CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$BOOST_HOME_2/include
export CPLUS_INCLUDE_PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST_HOME_2/lib
export LD_LIBRARY_PATH
LIBRARY_PATH=$LIBRARY_PATH:$BOOST_HOME_2/lib
export LIBRARY_PATH

if [[ $# -ge 2 ]];then
input=$1
output=$2

#output='test'
g++ -std=c++11 -I $BOOST_HOME_2/include  -lboost_thread -lpthread -g -o $output $input
echo "success!!"
fi
