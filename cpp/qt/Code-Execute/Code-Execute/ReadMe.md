# Replace cerr/cout --> QTextStream  

Search keyword **==QTextStream==**  
The result will be like the following list : 

1. cmdOptions.cpp 
1. globalDirector.cpp 
1. tokenMgr.cpp 
1. dataTypeUtil.cpp 
1. variblePool.cpp 


Search keyword **==QTextStream==**  inside the previous source code :

And then modified some code around  =="cerr"==  or =="cout"== 

```C++
//
// variblePool.h
//

+ #include <QString>

-    void    printAllVaribles(unsigned int flag);
+    QString printAllVaribles(unsigned int flag);


```



```C++
//
// myException.h
//

+ void setChInfo(ChInfo cur);
+ bool hasCursorInfo(ChInfo* pChInfo) const;


```



```C++
//
// mainwindow.cpp
//

void MainWindow::on_pushButton_clicked() 
{
    ...
    + director.setTextEditor( ui->outputEdit );
    ...
}
```



```C++


    token->getTokenContent()
    token->getTokenContent().c_str()


```
