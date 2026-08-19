# Issue List

## git pull / clone 
1. git pull/clone   <font size="5" color="red">Failed</font>

The reason is shown as below : 
> ssh: connect to host github.com port 22
> Connection timed out fatal: Could not read from remote repository. 



[github official solution](https://docs.github.com/en/authentication/troubleshooting-ssh/using-ssh-over-the-https-port)

Please add port 443 into the trust list:
Create a new config file named "config" into the following path : 

***==~/.ssh/config==*** 

and type the following script

```config
Host github.com
    Hostname ssh.github.com
    Port 443
    User git
```






# git with VPN Support

git config VPN proxy settings : (关于clone github 中的仓库，一直失败，是因为VPN的代理没有被应用到 git bash的终端，需要进行如下配置的其中某1种 )
```bash
     $ git config --global http.proxy    socks5   127.0.0.1:portID
     $ git config --global https.proxy    socks5   127.0.0.1:portID
```

or

```bash
     $ git config --global http.proxy 127.0.0.1:portID
     $ git config --global https.proxy 127.0.0.1:portID
```

	 
目前我的当前设置为 PortID = 10080 ( 非 socks5 协议)
 
## 可以通过以下命令，查看已经配置完成的列表 
```bash
        $  git config --global --list
```

## 查看某一个指定的单项配置
```bash
        $ git config --global     --get     https.proxy
```


## 清除某一个指定的已经设置过的单项配置
```bash
        $ git config --global     --unset     https.proxy
```

## How to solve git-bash display chinese character garbled issue ? 
Also see the following website : 

url : [Chinese Character Display Garbled](https://gist.github.com/nightire/5069597)

中文乱码的根源在于 windows 基于一些历史原因无法全面支持 utf-8 编码格式，并且也无法通过有效手段令其全面支持。


2.1. 让 Git 支持 utf-8 编码

在命令行下输入以下命令：

$ git config --global core.quotepath false  		# 显示 status 编码
$ git config --global gui.encoding utf-8			# 图形界面编码
$ git config --global i18n.commit.encoding utf-8	# 提交信息编码
$ git config --global i18n.logoutputencoding utf-8	# 输出 log 编码
$ export LESSCHARSET=utf-8 

\# 最后一条命令是因为 git log 默认使用 less 分页，所以需要 bash 对 less 命令进行 utf-8 编码


