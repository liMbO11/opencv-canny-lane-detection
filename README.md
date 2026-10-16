# OpenCV Canny 车道线检测

一个基于 Qt Widgets 与 OpenCV 的车道线检测演示程序，支持摄像头采集，并通过独立的图像处理对象处理视频帧。

## 处理流程

每帧依次进行高斯模糊、灰度转换、Canny 边缘检测；随后使用多边形掩膜保留道路感兴趣区域，通过概率霍夫变换提取线段，并按斜率绘制左右车道线。图像处理对象移动到工作线程，处理结果再通过 Qt 信号槽返回界面。

## 技术栈

- C++11、Qt 5 Widgets、QTimer、Qt 信号与槽及 QThread。
- OpenCV：VideoCapture、图像滤波/变换、Canny、掩膜与 HoughLinesP。
- qmake 项目文件：`line_det.pro`。

## 构建

使用 Qt Creator/qmake 打开 `line_det.pro`。需安装与目标架构匹配的 Qt 和 OpenCV 开发库；项目文件中包含 IMX6ULL 工具链路径示例，使用时请按本机 SDK 路径调整。

## 限制

当前 ROI 顶点与 Hough 参数按示例画面设置。更换摄像头分辨率、视角或道路环境后，通常需要重新标定 ROI 和阈值。本仓库未附带测试视频或训练数据。
