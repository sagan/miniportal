
# miniportal

一个极简的 C 写的单可执行文件零（外部）依赖 [Captive portal](https://en.wikipedia.org/wiki/Captive_portal) 程序原型。

本程序适合放在家用路由器上跑，在斐讯 K2 路由潘多拉固件（PandoraBox 17.09 R7.3 Mod By LEAN）上测试通过。

本程序特点:

* 只需要主程序 + 一些静态文件即可在设备上运行。
* 主程序是单个可执行文件，（除标准C库）无任何依赖。
* 主程序编译后大小仅为几十KB。加上静态文件后整个程序大小不超过 200KB。
* 对设备系统要求低，只需要有 ipset + iptables 即可跑。
* 零配置。

# Requirements

运行设备需要有 ipset（需要支持 hash:ip）、iptables（需要支持 -m mac）。

# Deploy & Run

1. 编译程序主文件 miniportal。

2. 将编译出的 miniportal 可执行文件和 ````__files__```` 文件夹一起放到路由器上某个（相同）目录。

3. 在这个目录执行 ./miniportal 即可。

# Config

## 白名单

可以设置无需验证即可上网的白名单，有两种方式。需要每次设备启动时执行一次脚本。

### 1. IP 白名单（需要系统 iptables 支持 -m set）

```
ipset create portal_wl hash:ip
ipset add portal_wl 1.2.3.4
```

### 2. Mac 白名单

```
iptables -t nat -N portal_prerouting_pre
iptables -t nat -A portal_prerouting_pre -m mac --mac-source 00:00:00:00:00:00 -j RETURN
```


