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

