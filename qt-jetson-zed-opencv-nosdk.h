#ifndef QT_OPENCV_ZED_DEMO_H_
#define QT_OPENCV_ZED_DEMO_H_

#include <QMainWindow>
#include <QLabel>
#include <QTime>
#include "qoclbm_params.h"
#include "qopencvscene.h"

#include <opencv2/highgui/highgui.hpp>

namespace Ui {
    class QtOpenCVZedDemo;
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

#ifndef Q_PROCESSOR_ARM
    void doStereoSBM_OCL( cv::Mat left, cv::Mat right );
    void doStereoCSBP_OCL( cv::Mat left, cv::Mat right );
    void doStereoBP_OCL( cv::Mat left, cv::Mat right );
#endif

    void on_tabWidget_currentChanged(int index);

private:
    Ui::QtOpenCVZedDemo *ui;

    cv::VideoCapture mCapture;

    void updateFps( float time );
    void resetFps();

protected:
    void timerEvent(QTimerEvent *event);

private:
    bool mFlipVert;
    bool mFlipHoriz;

    bool mCuda;
    bool mOcl;

    cv::Mat mDisparity;
    
    QOclBM_params* mOclBmWidget;

    QLabel mFpsLabel;

    QTime mElabTime;
    float mFpsVec[10];
    int mFpsCount;
    int mFpsIdx;
    float mFpsSum;

    QOpenCVScene mLeft;
    QOpenCVScene mRight;
    QOpenCVScene mDisp;
};

#endif // QT_OPENCV_ZED_DEMO_H_

