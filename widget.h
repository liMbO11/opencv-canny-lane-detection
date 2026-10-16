#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include <QTimer>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>


#include "camera.h"
#include "opencv2/imgproc.hpp"
#include "opencv/cv.h"
#include "opencv2/imgproc/imgproc.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE
using namespace cv;
using namespace std;

class Camera;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QImage saveImage;
    Camera *camera;

    //初始化布局
    void layoutInit();

    //扫描摄像头设备
    void scanCameraDevice();

    cv::VideoCapture *videoCapture;

    QTimer *videoTimer;

    QQueue<cv::Mat> frameBuffer;  // 用于缓存视频帧
    QMutex bufferMutex;           // 保护缓存的互斥量
    bool isBuffering = false;     // 标志是否正在缓存

private slots:
    void showImage(const QImage&);
    void showFrame();
    void processNextFrame();

    void on_pushButton1_clicked(bool checked);
    void on_pushButton0_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_play_clicked();
};
#endif // WIDGET_H
