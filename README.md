# SURF-HLS
2020年新⼯科联盟-Xilinx暑期学校（Summer School）项⽬

## 使用HLS实现SURF算法

### 一，项目介绍

本项目旨在利用HLS实现SURF算法, 目前已实现特征点检测的仿真，在**Sourcecode\source\mSURF.cpp**中添加DEBUG宏定义以输出各阶段运算数据

#### 1，文件说明

**images**目录为空

**ExecutableFiles**目录为空

**Sourcecode\common\images**文件夹里的**1.png**为测试用的图片

**Sourcecode\source**文件夹下为HLS源文件，

​		**top.c**为顶层文件

​		**test.c**为测试文件

**Sourcecode\common\OutputFile**文件夹下为DEBUG输出的文件，主要文件说明如下

​		**sum.txt**积分图数据

​		**HBox.txt**为生成的hessian模板

​		**det0.txt**~**det8.txt**为hessian模板遍历积分图的输出数据

​		**keyPoint.txt**为检测到的特征点在原图中的坐标

2，项目目录

![项目目录结构](F:\MyGit\SURF-HLS\images\项目目录结构.png)

#### 2，工具版本

vivado HLS 2018.3





