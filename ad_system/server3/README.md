1. 当网页类型为txt,xml时，使用自定义处理流程(需要继承类`http::CoreWorkInterface`)
2. 否则读取静态文件（html,htm,jpg,png,gif）;默认为html
3. 静态文件默认路径为程序运行目录下的`wwwroot`文件
4. `http::server3::server (<address> <port> <threads> <doc_root><extensionType>)`

