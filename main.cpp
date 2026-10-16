#include <QApplication>
#include <QMetaType>
#include <opencv2/opencv.hpp>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat>("cv::Mat");  // 在 main 函数中注册 cv::Mat 类型

    Widget w;
    w.show();
    return a.exec();
}
