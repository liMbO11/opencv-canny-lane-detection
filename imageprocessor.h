#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H


#include <QObject>
#include <opencv2/opencv.hpp>

class ImageProcessor : public QObject {
    Q_OBJECT

public:
    explicit ImageProcessor(QObject *parent = nullptr);
    ~ImageProcessor();

public slots:
    void processFrame(const cv::Mat &frame);

signals:
    void frameProcessed(const cv::Mat &frame);

};

#endif // IMAGEPROCESSOR_H
