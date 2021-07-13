# Radar - 哈工大深圳南工骁鹰战队2021赛季雷达站代码

本代码是运行在雷达站上的系统，*预计*将有以下功能：

1. 敌方机器人位置检测
2. 敌方机器人类型判断
3. 操作手小地图绘制
4. 机器人行动轨迹预测
5. *机器人动作预判*
6. 危险警报预警
7. *自动步兵协助*

## Develop

### 运行流程

*暂时见：*`main.cpp`

### 通信协议

向操作手电脑绘图协议：[协议文档以及示例程序](https://github.com/RoboMaster/referee_serial_port_protocol)

### 代码规范

命名空间：全小写，放在`rm`命名空间内

函数名：使用首字母小写的驼峰命名法

类型名：使用首字母大写的驼峰命名法，如果在命名空间内就全小写

变量名：使用下划线分割命名法

大括号：不换行

缩进：两个空格

### 文件说明

```
├─.idea
│  └─codeStyles
├─cmake-build-debug
│  └─ # 编译会自动生成的文件
├─include # 公共头文件
│  ├─radar.hpp # 公用头文件，引用了整个工程所有的类型
│  └─instance.cpp # 存放全局变量实例
├─processing # 其他语言做的数据处理
├─programs # 程序入口
├─resource # 程序运行资源，构建的时候会自动复制到cmake-build-debug
└─src # 工程库源码
    ├─ai # 可能包含自动预警等
    ├─com # 串口库
    ├─mapping # 摄像头点映射到小地图点
    ├─minimap #小地图绘图逻辑
    ├─others # 其他库
    ├─rmcamera #摄像头库
    └─rmconfig # 配置加载保存
```



### TODO-List

#### Today's

- [x] 重构：更改工程架构和计划架构
- [x] 架构：在这些过程中注意工程架构
- [x] 修改：写相机驱动wrapper
- [x] 规划：雷达站各种常量
- [x] 构建：构建自己的config加载模块
- [x] 构建：增加串口读写部分
- [ ] 构建：小地图图传输出格式
- [ ] 对接：和电控对接自定义UI绘图
- [ ] 优化：KD树可能出现的问题
- [ ] 优化：非离散
- [ ] 优化：三角形
- [ ] 优化：标定程序的效率
- [ ] 实施：尝试初步标定
- [ ] 修改：小地图UI样式
- [ ] 精简：小地图UI逻辑

#### Future's

- [ ] 计划：多个摄像头的情况
- [ ] 计划：自动预警信息
- [ ] 计划：识别摄像头中的机器人
- [ ] 计划：和其他机器人配合确定敌人的机器人类型
- [ ] 计划：实际在裁判系统和操作手UI中调试

## Release

### 代码运行环境

| 硬件设备 | 操作系统             | 运行库                                            | ToolChain |
| -------- | -------------------- | ------------------------------------------------- | --------- |
| *未定*   | Linux (*发行版未定*) | OpenCV4.5.1 opencv_contrib4.5.1 大恒相机驱动 GLOG | cmake     |

相机驱动下载地址：[相机驱动](https://www.daheng-imaging.com/)

OpenCV下载地址：[OpenCV](https://github.com/opencv)

OpenCV安装教程 : [linux](https://docs.opencv.org/3.4.11/d7/d9f/tutorial_linux_install.html)  [Windows](https://docs.opencv.org/3.4.11/d3/d52/tutorial_windows_install.html)

GLOG : [GLOG](https://github.com/google/glog)

### 编译运行方式

### 文件目录结构

### 关键类解析

