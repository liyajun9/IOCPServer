简单的socket服务器,基于IOCP, 使用3个线程池分别处理IO请求/完成，转发，请求处理.

├─BasicServer
└─src
    ├─basicServer
    │  ├─buffer			IO使用的buffer, 预分配对象内存，扩展了OVERLAPP
    │  ├─cachedObj		实现内存预分配的类模板
    │  ├─connection		保存socket连接相关数据
    │  ├─protocol		简单的报文格式
    │  └─threadPool		用于服务器的线程池，包含处理IO工作，转发请求，处理请求三种线程池
    │      ├─IOCP
    │      └─workerThread	对应线程池的工作线程
    ├─exceptions		异常
    ├─log				日志
    ├─macros			常用宏
    └─utils				常用工具
        ├─database
        ├─encodings
        ├─synchronize
        ├─thread
        ├─time
        └─trace
