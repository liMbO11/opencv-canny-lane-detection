// camera.cpp
#include "camera.h"
#include <QMetaType>
#include <QImage>
#include <QDebug>

Q_DECLARE_METATYPE(cv::Mat)

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<cv::Mat>("cv::Mat");  // 注册 cv::Mat 类型

    capture = new cv::VideoCapture();
    timer = new QTimer(this);
    imageProcessor = new ImageProcessor();
    imageProcessor->moveToThread(&processorThread);

    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeOut()));
    connect(this, &Camera::newFrameCaptured, imageProcessor, &ImageProcessor::processFrame);
    connect(imageProcessor, &ImageProcessor::frameProcessed, this, &Camera::handleProcessedFrame);

    processorThread.start();
}

Camera::~Camera(){
    processorThread.quit();
    processorThread.wait();
    delete capture;
    capture = NULL;
    delete imageProcessor;
}

//选择摄像头设备
void Camera::selectCameraDevice(int index){
    if (capture->isOpened()) {
        capture->release();
    }
    capture->open(index);
}

//开启定时器
bool Camera::cameraProcess(bool bl){
    if (bl) {
        timer->start(250);
    } else {
        timer->stop();
    }
    return capture->isOpened();
}

void Camera::timerTimeOut(){
    if (!capture->isOpened()) {
        timer->stop();
        return;
    }
    cv::Mat frame;
    *capture >> frame;
    if (!frame.empty()) {
        emit newFrameCaptured(frame); // 发送帧到处理器线程进行处理
    }
}
void Camera::handleProcessedFrame(const cv::Mat &frame) {
    if (!frame.empty()) {
        emit readyImage(matToQImage(frame));  // 将处理后的图像发送到 UI 进行显示
    }
}

QImage Camera::matToQImage(const cv::Mat &img){
    //rgb三通道摄像头
    if(img.type() == CV_8UC3) {
        const uchar *pimg =(const uchar*)img.data;
        QImage qImage(pimg, img.cols, img.rows, img.step, QImage::Format_RGB888);
        return qImage.rgbSwapped();
    }
    return QImage();
}
