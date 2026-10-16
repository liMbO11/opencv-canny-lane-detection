// widget.cpp
#include "widget.h"
#include "ui_widget.h"
#include <QGuiApplication>
#include <QScreen>
#include <QFile>
#include <QPixmap>
#include <QBuffer>
#include <QFileDialog>
#include "camera.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("基于嵌入式linux和opencv的车道线监测系统");
    layoutInit();
    scanCameraDevice();
}

Widget::~Widget() {
    delete ui;
}

void Widget::layoutInit() {
    QList<QScreen *> list_screen = QGuiApplication::screens();
#if __arm__
    this->resize(list_screen.at(0)->geometry().width(), list_screen.at(0)->geometry().height());
#else
    this->resize(800, 480);
#endif

    ui->pushButton0->setEnabled(false);
    ui->pushButton1->setCheckable(true);

    camera = new Camera(this);
    videoCapture = new cv::VideoCapture();
    videoTimer = new QTimer(this);

    connect(camera, SIGNAL(readyImage(QImage)), this, SLOT(showImage(QImage)));
    connect(ui->pushButton1, SIGNAL(clicked(bool)), camera, SLOT(cameraProcess(bool)));
    connect(videoTimer, SIGNAL(timeout()), this, SLOT(showFrame()));
}

void Widget::scanCameraDevice() {
    QFile file("/dev/video0");
    if (file.exists())
        ui->comboBox->addItem("video0");
    else {
        ui->displayLabel->setText("无摄像头设备");
        return;
    }

    file.setFileName("/dev/video1");
    if (file.exists()) {
        ui->comboBox->addItem("video1");
        ui->comboBox->setCurrentIndex(1);
    }

    file.setFileName("/dev/video2");
    if (file.exists())
        ui->comboBox->addItem("video2");

#if !__arm__
    ui->comboBox->addItem("video0");
#endif
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), camera, SLOT(selectCameraDevice(int)));
}

void Widget::showImage(const QImage &image) {
    if (!image.isNull()) {
        ui->displayLabel->setPixmap(QPixmap::fromImage(image));
        saveImage = image;
        ui->pushButton0->setEnabled(true);
    } else {
        ui->pushButton0->setEnabled(false);
    }
}

void Widget::showFrame() {
    if (videoCapture->isOpened()) {
        cv::Mat currentFrame;
        if (videoCapture->read(currentFrame)) {
            QMutexLocker locker(&bufferMutex);
            frameBuffer.enqueue(currentFrame);  // 将读取的帧放入缓存
        }
    }

    if (!isBuffering) {
        processNextFrame();  // 如果不在缓冲中，处理缓存中的下一帧
    }
}

void Widget::processNextFrame() {
    QMutexLocker locker(&bufferMutex);
    if (!frameBuffer.isEmpty()) {
        cv::Mat frame = frameBuffer.dequeue();  // 从缓存中取出一帧

        // 处理帧并显示
        if (!frame.empty()) {
            emit camera->newFrameCaptured(frame);  // 发送帧到处理线程
        }
    }
    // 确保处理下一帧
    QTimer::singleShot(0, this, &Widget::processNextFrame);
}

void Widget::on_pushButton1_clicked(bool checked) {
    if (checked) {
        videoTimer->stop();
        videoCapture->release();
        camera->selectCameraDevice(ui->comboBox->currentIndex());
        ui->lineEdit_log->setText("camera is playing");
        ui->pushButton1->setText("关闭");
    } else {
        ui->pushButton0->setEnabled(false);
        ui->pushButton1->setText("开始");
    }
}

void Widget::on_pushButton0_clicked() {
    if (!saveImage.isNull()) {
        QString fileName = QCoreApplication::applicationDirPath() + "/test.png";
        saveImage.save(fileName, "PNG", -1);
        ui->saveLabel->setPixmap(QPixmap::fromImage(QImage(fileName)).scaled(ui->saveLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void Widget::on_pushButton_open_clicked() {
    QString videoPath = QFileDialog::getOpenFileName(this, "打开视频文件",
                                                     QCoreApplication::applicationDirPath(),
                                                     "AllFile(*.*);;"
                                                     "MP3(*.mp3);;"
                                                     "MP4(*.mp4);;"
                                                     "MKV(*.mkv);;"
                                                     "AVI(*.AVI)");
    ui->lineEdit_path->setText(videoPath);
    ui->lineEdit_log->setText("video is loaded");
}

void Widget::on_pushButton_play_clicked() {
    ui->displayLabel->clear();
    QString videoPath = ui->lineEdit_path->text();
    videoCapture->open(videoPath.toStdString());

    double rate = videoCapture->get(cv::CAP_PROP_FPS);
    double videoDelay = 1000 / rate;
    videoTimer->start(videoDelay);
}

