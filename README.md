
# miniportal

一个简单的 C 写的单可执行文件零（外部）依赖 [Captive portal](https://en.wikipedia.org/wiki/Captive_portal) 程序原型。

** 玩具性质，不建议用于正式环境。**

# Requirements

运行设备需要有 ipset、iptables。

# Deploy

1. 编译程序主文件 miniportal。

2. 将编译出的 miniportal 可执行文件和 __files__ 文件夹一起放到路由器上某个（相同）目录。

3. 在这个目录执行 ./miniportal 即可。

