# 通过kill命令发送信号给进程来开启/关闭性能分析：
1. 用top命令查看进程的PID
2. kill -s SIGUSR1 PID //第一次运行命令启动性能分析
3. kill -s SIGUSR1 PID //再次运行命令关闭性能分析，产生test.prof
4. 生成pdf分析报告: `pprof --pdf a.out test.prof >test.pdf`
5. 生成text的分析报告`pprof --text a.out test.prof`
