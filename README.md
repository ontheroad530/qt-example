# qt-example

## 开发环境：VS2019+QT5.15.2

## 子项目

* **zipcodes**
>代码来源：书籍《Qt高级编程》第3章：模型/视图表格模型  
> 知识点：  
>	1、模型、表格视图、代理 编码范例  
>	2、表格内容编辑时显示（代理）ItemDelegate：编辑时显示成 QSPinxBox、QCombbox  
>	3、去重显示 UniqueProxyModel  
>	4、过滤代理：ProxyModel  
>   
>	其它：内容修改后 显示*，提示保存setWindowModified setWindowTitle[*]      

* **pagedesigner**
>代码来源：书籍《Qt高级编程》 第12章 创建图形/视图场景  
>知识点：  
>  创建自定义图形项：介绍了三种不同类型的自定义图形项  



* **excel**  
>qt操作excel，使用QAxObject需要依赖安装 excel程序  
>因此使用QtXlsx进行excel操作  
>参考https://blog.csdn.net/zhengyanan815/article/details/132274988  


* **splash**  
>程序启动欢迎画面，或启动等待提示画面  
>来源：QT帮助文档，搜索QSplashScreen  


* **CommandLine**  
>使用QLineEidt模拟命令行功能，上\下键历史记录，CTRL+L清空历史记录，CTRL+V黏贴  
>来源：参考QCAD RCommandLine实现  

* **Image2Image**  
>使用 图片格式转换  
>来源：参考书籍《Qt高级编程》 第7章 使用QtConcurrent实现线程处理  


* **QtRO : Qt Remote Objects**  
>子项目： qtrosource 服务， qtroreplica 静态客户端， dynamicclient 动态客户端  
>使用 IPC RPC  
>来源：QT官网文档  


* **timeschedule**  
>使用 时间刻度尺（可拖拽、缩放、新增）    
>来源：自己实现  


* **daemon**  
>子项目：childproc 子进程， daemonproc 守护进程  
>来源：自己实现  
>功能： 守护进程监控子进程状态，崩溃自启动  

* **arpscan**      
>来源：自己实现    
>功能： 扫描局域网网段IP/MAC   


* **colordialog**
>取色器实现

