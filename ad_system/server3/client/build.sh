input=$1
output='http_client'
g++ -std=c++11 -lboost_thread -lboost_regex -lpthread -o $output $input -g -fno-elide-constructors
