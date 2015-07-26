#include "qt-jetson-zed-opencv-nosdk.h"
#include "ui_qt-jetson-zed-opencv-nosdk.h"

#include <opencv2/gpu/gpu.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <opencv2/calib3d/calib3d.hpp>

QtOpenCVZedDemo::QtOpenCVZedDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtGLWebcamDemo)
{
    ui->setupUi(this);

    mFlipVert=true;
    mFlipHoriz=false;

    ui->actionVertical_Flip->setChecked( mFlipVert );
    ui->action_Horizontal_Mirror->setChecked( mFlipHoriz );

    enumCameras();

    mCuda = checkCUDA();
    mOcl = checkOpenCL();

    ui->radioButton_cuda->setEnabled( mCuda );
    ui->radioButton_opencl->setEnabled( mOcl );
}

QtOpenCVZedDemo::~QtOpenCVZedDemo()
{
    delete ui;
}

void QtOpenCVZedDemo::enumCameras()
{
    ui->comboBox_cameras->clear();

    if(mCapture.isOpened())
        mCapture.release();

    cv::VideoCapture temp_camera;
    int maxTested = 10;

    bool found = false;

    for (int i = 0; i < maxTested; i++)
    {
        cv::VideoCapture temp_camera(i);
        bool res = temp_camera.isOpened();
        temp_camera.release();
        if (res)
        {
            found = true;
            ui->comboBox_cameras->addItem( tr("%1").arg(i) );
        }
    }

    if( found )
        ui->comboBox_cameras->setCurrentIndex(0);
    else
        ui->comboBox_cameras->setCurrentIndex(-1);
}

bool QtOpenCVZedDemo::checkCUDA()
{
    // >>>>> CUDA available?
    int cudaCount = cv::gpu::getCudaEnabledDeviceCount();

    bool cuda = cudaCount>0?true:false;
    // <<<<< CUDA available?

    return cuda;
}

bool QtOpenCVZedDemo::checkOpenCL()
{
    // >>>>> OPENCL available?
    cv::ocl::PlatformsInfo oclPlat;
    int oclCount = cv::ocl::getOpenCLPlatforms( oclPlat );

    bool ocl = oclCount>0?true:false;
    // <<<<< OPENCL available?

    return ocl;
}

void QtOpenCVZedDemo::on_actionStart_triggered()
{
    if( !mCapture.isOpened() )
        if( !mCapture.open( ui->comboBox_cameras->currentIndex() ) )
            return;

    startTimer(0);
}

void QtOpenCVZedDemo::timerEvent(QTimerEvent *event)
{
    cv::Mat image;
    mCapture >> image;

    if( mFlipVert && mFlipHoriz )
        cv::flip( image,image, -1);
    else if( mFlipVert )
        cv::flip( image,image, 0);
    else if( mFlipHoriz )
        cv::flip( image,image, 1);

    // Do what you want with the image :-)
    int w = image.cols/2;
    int h = image.rows;

    cv::Rect lRect( 0,0,w,h );
    cv::Rect rRect( w,0,w,h );

    cv::Mat left = image(lRect);
    cv::Mat right = image(rRect);

    // Show the image
    ui->openCVviewer_L->showImage( left );
    ui->openCVviewer_R->showImage( right );

    if( ui->radioButton_cpu->isChecked() )
    {
        doStereoSGBM_CPU( left, right );
    }
    else if( ui->radioButton_opencl->isChecked() )
        doStereoSBM_OCL(left, right );

    ui->openCVviewer_D->showImage( mDisparity );
}

void QtOpenCVZedDemo::on_actionVertical_Flip_triggered(bool checked)
{
    mFlipVert = checked;
}

void QtOpenCVZedDemo::on_action_Horizontal_Mirror_triggered(bool checked)
{
    mFlipHoriz = checked;
}

void QtOpenCVZedDemo::on_comboBox_cameras_currentIndexChanged(int index)
{
    if( index==-1 )
        return;

    if( mCapture.isOpened() )
        mCapture.release();

    if( !mCapture.open( index ) )
        return;

    mCapture.set( CV_CAP_PROP_FRAME_WIDTH, 1280 );
    mCapture.set( CV_CAP_PROP_FRAME_HEIGHT, 480 );

    startTimer(0);
}

void QtOpenCVZedDemo::doStereoSGBM_CPU( cv::Mat left, cv::Mat right )
{
    cv::StereoSGBM sgbm;

    sgbm.SADWindowSize = 5;
    sgbm.numberOfDisparities = 192;
    sgbm.preFilterCap = 4;
    sgbm.minDisparity = -64;
    sgbm.uniquenessRatio = 1;
    sgbm.speckleWindowSize = 150;
    sgbm.speckleRange = 2;
    sgbm.disp12MaxDiff = 10;
    sgbm.fullDP = true;
    sgbm.P1 = 600;
    sgbm.P2 = 2400;

    if( left.channels() > 1  )
        cv::cvtColor( left, left, CV_BGR2GRAY );

    if( right.channels() > 1  )
        cv::cvtColor( right, right, CV_BGR2GRAY );

    sgbm(left, right, mDisparity);
    normalize(mDisparity, mDisparity, 0, 255, CV_MINMAX, CV_8U);
}

void QtOpenCVZedDemo::doStereoSBM_OCL( cv::Mat left, cv::Mat right )
{
    cv::ocl::StereoBM_OCL sbm;

    sbm.preset = cv::ocl::StereoBM_OCL::BASIC_PRESET;
    sbm.ndisp = 5*16;
    sbm.winSize = 5;

    if( left.channels() > 1  )
        cv::cvtColor( left, left, CV_BGR2GRAY );

    if( right.channels() > 1  )
        cv::cvtColor( right, right, CV_BGR2GRAY );

    cv::ocl::oclMat ocl_left;
    cv::ocl::oclMat ocl_right;
    cv::ocl::oclMat ocl_disp;

    ocl_left.upload( left );
    ocl_right.upload( right );

    sbm( ocl_left, ocl_right, ocl_disp );

    ocl_disp.download( mDisparity );
    normalize(mDisparity, mDisparity, 0, 255, CV_MINMAX, CV_8U);
}
