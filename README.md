# Server

## 祖产服务器设计 ANCESTRAL-PROPERTY-SERVER

### 环境搭建
* Protobuf : 3.19.1
* CMake    : 3.3.2
* MySQL    : Remote
* Redis    : Remote

### 启动方式
1. 进入src目录
2. 命令行执行：cmake ..
3. 命令行执行 make

### 添加一个Protobuffer需要注意的地方
1. 服务端
* MessageUtils.h new对应类型的body->message, 添加对应的New函数
* 对应Server的OnMsgBodyAnalysised添加case
* Profile.h 添加enum类型
2. 客户端
* ServerMgr AnalysisMessage添加case, CreateMessageBody中记得new对应包体
* 需要分发给业务的消息发送到MessageFamily队列
* MessageHandler 添加处理函数
* NetDefines 添加enum类型

$*&^$*^$(^$(^$$^$$%*^&*)&)

2023.3.31 01:16 我真的是麻了,改的东西太多了，等全部改完都不知道能编译出什么牛马。。。 不过改完后自己用起来会相当舒服,希望别太离谱

* SendMsg不需要重载，一律使用SendMsg(Message*, int)
* 所有的SerializeServerInfoToArray全都删掉，最终的代码不应有ProtoUtils.cpp，应替换为MessageUtils.cpp
* Source下需要改的东西很多，还是稳一点好，轻易别gdb
顶不住了，先睡了


2023.3.31 15:48 终于解决完编译问题了，现在准备连接一下客户端和服务端，检查是否有牛马问题。。。

2023.3.31 16:53 完了，多线程的段错误，寄

2023.3.31 18:42 没有SegmentError了
* ulimit -a , -c 
* gdb xxxServer core.xxxxx 
* bt命令(进入gdb看segError的调用栈)

2023.4.3 01:24 终于找到了小黑子露出鸡脚的地方了呜呜呜。少写一个感叹号的代价：调试两天&虚函数博客看穿&学习gdb调试多线程（太麻烦了）&Clion远程调试&对着Log文件一顿看

2023.4.3 02:23 现在的版本功能基本正常，可以commit了

2023.4.4 16.24 服务器支持登录注册，注册判断表中是否存在该用户名。加入房间功能，可以支持加入房间、创建房间、离开房间、获取房间列表。

2023.4.12 23.09 好久没更新了，现在功能相对完善了(房间待查改，还有一点逻辑问题)。泪目T_T……，如果客户端有脑瘫问题找不到直接try-catch。
