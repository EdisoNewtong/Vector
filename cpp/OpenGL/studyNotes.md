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

### 你好，窗口 
**双缓冲(Double Buffer)**

应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。 
这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。
最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。
为了规避这些问题，我们应用双缓冲渲染窗口应用程序。
**==前==**缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在**==后==**缓冲上绘制。
当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。


### 你好，三角形
- VAO     : 顶点数组对象：Vertex Array Object
- VBO     : 顶点缓冲对象：Vertex Buffer Object
- EBO/IBO : 元素缓冲对象：Element Buffer Object，EBO 或 索引缓冲对象 Index Buffer Object

**==OpenGL中的一个片段是OpenGL渲染一个像素所需的所有数据。==**
- NDC : 标准化设备坐标(Normalized Device Coordinates) 

OpenGL仅当3D坐标在3个轴（x、y和z）上-1.0到1.0的范围内时才处理它。所有在这个范围内的坐标叫做标准化设备坐标(Normalized Device Coordinates)
通过使用由glViewport函数提供的数据，进行视口变换(Viewport Transform)，标准化设备坐标(Normalized Device Coordinates)会变换为屏幕空间坐标(Screen-space Coordinates)

Viewport Transform --> Normalized Device Coordinates -->  Screen-Space Coordinates

从CPU把数据发送到显卡相对较慢，所以只要可能我们都要尝试尽量一次性发送尽可能多的数据。



在OpenGL中绘制一个物体，代码会像是这样：

```c++
// 0. 复制顶点数组到缓冲中供OpenGL使用
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 1. 设置顶点属性指针
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// 2. 当我们渲染一个物体时要使用着色器程序
glUseProgram(shaderProgram);
// 3. 绘制物体
someOpenGLFunctionThatDrawsOurTriangle();
```


每当我们绘制一个物体的时候都必须重复这一过程。这看起来可能不多，
但是如果有超过5个顶点属性，上百个不同物体呢（这其实并不罕见）。
绑定正确的缓冲对象，为每个物体配置所有顶点属性很快就变成一件麻烦事。
有没有一些方法可以使我们把所有这些状态配置储存在一个对象中，并且可以通过绑定这个对象来恢复状态呢？

** 顶点数组对象 **
顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。这样的好处就是，当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了。刚刚设置的所有状态都将存储在VAO中

> 
> OpenGL的核心模式要求我们使用VAO，所以它知道该如何处理我们的顶点输入。如果我们绑定VAO失败，OpenGL会拒绝绘制任何东西。
>
