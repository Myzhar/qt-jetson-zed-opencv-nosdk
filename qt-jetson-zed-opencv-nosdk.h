#ifndef QTGLWEBCAMDEMO_H
#define QTGLWEBCAMDEMO_H

#include <QMainWindow>

#include <opencv2/highgui/highgui.hpp>

namespace Ui {
    class QtGLWebcamDemo;
}

class QtOpenCVZedDemo : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtOpenCVZedDemo(QWidget *parent = 0);
    ~QtOpenCVZedDemo();

private slots:
    void on_actionStart_triggered();

    void on_actionVertical_Flip_triggered(bool checked);
    void on_action_Horizontal_Mirror_triggered(bool checked);

    void enumCameras();
    bool checkCUDA();
    bool checkOpenCL();

    void on_comboBox_cameras_currentIndexChanged(int index);

    void doStereoSGBM_CPU( cv::Mat left, cv::Mat right );
    void doStereoSBM_OCL( cv::Mat left, cv::Mat right );

private:
    Ui::QtGLWebcamDemo *ui;

    cv::VideoCapture mCapture;

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool mFlipVert;
    bool mFlipHoriz;

    bool mCuda;
    bool mOcl;

    cv::Mat mDisparity;
};

#endif // QTGLWEBCAMDEMO_H
