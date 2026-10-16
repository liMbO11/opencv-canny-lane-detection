// imageprocessor.cpp
#include "imageprocessor.h"

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent) {}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::processFrame(const cv::Mat &frame) {
    if (frame.empty()) return;

    cv::Mat gaussionFrame, grayFrame, cannyFrame;
    // 高斯模糊
    cv::GaussianBlur(frame, gaussionFrame, cv::Size(5, 5), 1, 0);
    // 灰度变换
    cv::cvtColor(gaussionFrame, grayFrame, cv::COLOR_BGR2GRAY);
    // Canny 边缘检测
    cv::Canny(grayFrame, cannyFrame, 50, 100);

    cv::Mat resultFrame;
    cv::Mat maskFrame = cv::Mat::zeros(cannyFrame.size(), cannyFrame.type());

    // 提取感兴趣区域
    std::vector<cv::Point> polyPoints = {
        cv::Point(50, 270),
        cv::Point(300, 200),
        cv::Point(330, 200),
        cv::Point(640, 270)
    };
    cv::fillPoly(maskFrame, std::vector<std::vector<cv::Point>>{polyPoints}, cv::Scalar(255, 255, 255));
    cv::bitwise_and(cannyFrame, maskFrame, resultFrame);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(resultFrame, lines, 1, CV_PI / 180, 5, 40, 20);

    for (size_t i = 0; i < lines.size(); i++) {
        cv::Vec4i carLine = lines[i];
        double lineSlope = (double(carLine[3]) - double(carLine[1])) / (double(carLine[2]) - double(carLine[0]));
        if (lineSlope > 0) {
            // 左车道线
            cv::line(frame, cv::Point(carLine[0], carLine[1]), cv::Point(carLine[2], carLine[3]), cv::Scalar(0, 255, 255), 5, cv::LINE_AA);
        } else if (lineSlope < 0) {
            // 右车道线
            cv::line(frame, cv::Point(carLine[0], carLine[1]), cv::Point(carLine[2], carLine[3]), cv::Scalar(0, 255, 255), 5, cv::LINE_AA);
        }
    }

    emit frameProcessed(frame);
}

