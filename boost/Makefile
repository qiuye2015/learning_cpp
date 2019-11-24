.SUFFIXES:.c .o # 说明.c和.o建立关联
CC=g++

#SRCS=hello.c\
	 		add.c	# 同时编译2个.c文件	
SRCS=3.chat_message.hpp\
	 3.chat_client.cpp
OBJS=$(SRCS:.c=.o) # 把.c替换成.o OBJS=hello.o add.o
	EXEC=char_client

start:$(OBJS)	# 依赖hello.o add.o
		$(CC) -std=c++11 -I $(BOOST_HOME_2)/include  -lboost_thread -lpthread -o $(EXEC) $(OBJS)
			@echo "----end----" # @表示不输出本行，只执行命令
.c.o:
		$(CC) -o $@ -c $< # 自动根据$(OBJS)中.o的数量自动循环编译
			
clean:
		rm -rf $(OBJS)	
