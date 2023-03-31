# GameServer

## 欢乐互娱游戏demo后端设计

### 环境搭建
* Protobuf : 3.19.1
* CMake    : 3.3.2
* MySQL    : Remote
* Redis    : Remote

### 启动方式
1. 进入src目录
2. 命令行执行：cmake ..
3. 命令行执行 make

2023.3.31 01:16 我真的是麻了,改的东西太多了，等全部改完都不知道能编译出什么牛马。。。
                不过改完后自己用起来会相当舒服,希望别太离谱

* SendMsg不需要重载，一律使用SendMsg(Message*, int)
* 所有的SerializeServerInfoToArray全都删掉，最终的代码不应有ProtoUtils.cpp，应替换为MessageUtils.cpp
* Source下需要改的东西很多，还是稳一点好，轻易别gdb

顶不住了，先睡了
---
2023.3.31 15:48 终于解决完编译问题了，现在准备连接一下客户端和服务端，检查是否有牛马问题。。。