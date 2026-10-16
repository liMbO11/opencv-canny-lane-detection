// camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <QImage>
#include <QTimer>
#include <QThread>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv/cv.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "imageprocessor.h"

namespace cv {
    class VideoCapture;
    class Mat;
}

class Camera : public QObject {

    Q_OBJECT
public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

signals:
    void readyImage(const QImage&);
    void newFrameCaptured(const cv::Mat&);

public slots:
    bool cameraProcess(bool);
    void selectCameraDevice(int);

private slots:
    void timerTimeOut();
    void handleProcessedFrame(const cv::Mat &frame);

private:
    cv::VideoCapture *capture;
    QTimer *timer;
    QImage matToQImage(const cv::Mat&);
    QThread processorThread;
    ImageProcessor *imageProcessor;
};

#endif // CAMERA_H
