
# miniportal

一个简单的 C 写的单可执行文件零（外部）依赖 [Captive portal](https://en.wikipedia.org/wiki/Captive_portal) 程序原型。

**玩具性质，不建议用于正式环境。**

# Requirements

运行设备需要有 ipset、iptables。

# Deploy

1. 编译程序主文件 miniportal。

2. 将编译出的 miniportal 可执行文件和 ````__files__```` 文件夹一起放到路由器上某个（相同）目录。

3. 在这个目录执行 ./miniportal 即可。

# Config

## 白名单

可以设置无需验证即可上网的白名单，有两种方式。需要每次设备启动时执行一次脚本。

### 1. IP 白名单

```
ipset create portal_wl hash:ip
ipset add portal_wl 1.2.3.4
```

### 2. Mac 白名单

```
iptables -t nat -N portal_prerouting_pre
iptables -t nat -A portal_prerouting_pre -m mac --mac-source 00:00:00:00:00:00 -j RETURN
```


