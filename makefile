.SUFFIXES:.c .o # 说明.c和.o建立关联
CC=g++

SRCS=client.cpp
#SRCS=server.cpp
OBJS=$(SRCS:.c=.o) # 把.c替换成.o OBJS=hello.o add.o
	EXEC=client
	#EXEC=server

start:$(OBJS)	# 依赖hello.o add.o
	$(CC) -o $(EXEC) -g $(OBJS)
	@echo "----end----" # @表示不输出本行，只执行命令
.c.o:
	$(CC) -o $@ -c $< # 自动根据$(OBJS)中.o的数量自动循环编译

clean:
	rm -rf $(OBJS)	
