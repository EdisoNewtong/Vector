# Learn OpenGL by the utility GLFW 3.4

[Learn OpenGL CN](https://learnopengl-cn.github.io/)
[Learn OpenGL Eng](https://learnopengl.com/)
[GLFW Official Website](https://www.glfw.org/download.html)


# 目录 
## 入门 
### OpenGL 

OpenGL自身是一个巨大的状态机(State Machine)：一系列的变量描述OpenGL此刻应当如何运行。OpenGL的状态通常被称为OpenGL上下文(Context)。
我们通常使用**==如下途径==**去更改OpenGL状态：
1. 设置选项，
1. 操作缓冲。
1. 最后，我们使用当前OpenGL上下文来渲染。


