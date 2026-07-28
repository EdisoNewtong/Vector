--------------------------------------------------
Repository Name : Vector
Description     : Vector Your Life
--------------------------------------------------

##################################################
# Basic
##################################################

```bash
# Generate  SSH-Key
$ ssh-keygen -t ed25519     -C "your_email@example.com"
$ ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```


```bash
# Launch ssh-agent
$ eval "$(ssh-agent -s)"
> Agent pid 59566
```


```bash
$ ssh-add ~/.ssh/id_xxxxx
# show the list of all existed ssh-key  
$ ssh-add -l                     
```

## git url
SSH   :   git@github.com:EdisoNewtong/Vector.git
https :   https://github.com/EdisoNewtong/Vector.git


##################################################
# Proxy Setting
##################################################

git config VPN proxy settings : (关于clone github 中的仓库，一直失败，是因为VPN的代理没有被应用到 git bash的终端，需要进行如下配置的其中某1种 )
```bash
     $ git config --global http.proxy    socks5   127.0.0.1:portID
     $ git config --global https.proxy    socks5  127.0.0.1:portID
```

or
```bash

     $ git config --global http.proxy 127.0.0.1:portID
     $ git config --global https.proxy 127.0.0.1:portID
```
	 
目前我的当前设置为 PortID = **==10080==** ( 非 socks5 协议 )  见VPN工具内的已经配置好的端口号  

