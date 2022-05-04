# SURF-HLS
GitHub地址：https://github.com/liuqiang-bit/SURF-HLS.git

作者：GUDONG		时间：2020/08/28		软件：Vivado HLS 2018.3

## 使用HLS实现SURF算法

### 一，项目介绍

本项目旨在利用HLS实现SURF算法, 目前已实现特征点检测的仿真，在**Sourcecode\source\mSURF.cpp**中添加DEBUG宏定义以输出各阶段运算数据

#### 1、文件说明

**Sourcecode\common\images**文件夹里的**1.png**为测试用的图片

**Sourcecode\source**文件夹下为HLS源文件，

​		**top.c**为顶层文件

​		**test.c**为测试文件

**Sourcecode\common\OutputFile**文件夹下为DEBUG输出的文件，主要文件说明如下

​		**sum.txt**积分图数据

​		**HBox.txt**为生成的hessian模板

​		**det0.txt**~**det8.txt**为hessian模板遍历积分图的输出数据

​		**keyPoint.png**为绘制的特征点

## 2、运行结果

![](F:\MyGit\SURF-HLS\images\1.png)

## 3、性能指标

> 资源占用更小，性能指标更好参考简化版本：https://github.com/liuqiang-bit/SURF_HLS_Simplified.git

<img src="F:\MyGit\SURF-HLS\images\2.png" style="zoom:50%;" />

1234

