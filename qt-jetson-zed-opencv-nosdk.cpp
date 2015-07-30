#include "qt-jetson-zed-opencv-nosdk.h"
#include "ui_qt-jetson-zed-opencv-nosdk.h"

#include <opencv2/gpu/gpu.hpp>

#ifndef Q_PROCESSOR_ARM
#include <opencv2/ocl/ocl.hpp>
#endif
#include <opencv2/calib3d/calib3d.hpp>

QtOpenCVZedDemo::QtOpenCVZedDemo(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtOpenCVZedDemo)
{
    ui->setupUi(this);

    mFlipVert=true;
    mFlipHoriz=false;

    ui->actionVertical_Flip->setChecked( mFlipVert );
    ui->action_Horizontal_Mirror->setChecked( mFlipHoriz );

    enumCameras();

    mCuda = checkCUDA();
#ifndef Q_PROCESSOR_ARM
    mOcl = checkOpenCL();
#else
    mOcl = false;
#endif
    
    /*QWidget* cpuW = new QWidget;
    QGridLayout* cpuLayout = new QGridLayout;
    cpuLayout->addWidget( cpuW );
    ui->tabWidget->addTab( cpuLayout, "CPU" );

    QWidget* cudaW = new QWidget;
    QGridLayout* cudaLayout = new QGridLayout;
    cudaLayout->addWidget( cudaW );
    ui->tabWidget->addTab( cudaLayout, "CUDA" );

    mOclBmWidget = new QOclBM_params;
    QGridLayout* oclLayout = new QGridLayout;
    oclLayout->addWidget( mOclBmWidget );
    ui->tabWidget->addTab( oclLayout, "OpenCL" );*/

    mOclBmWidget = qobject_cast<QOclBM_params *>(ui->tabWidget->widget(2));

    ui->tabWidget->setTabEnabled( 1, mCuda );
    ui->tabWidget->setTabEnabled( 2, mOcl );

    ui->statusBar->addPermanentWidget( &mFpsLabel );
    mFpsLabel.setText( "FPS: ---");
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
#ifndef Q_PROCESSOR_ARM
    // >>>>> OPENCL available?
    cv::ocl::PlatformsInfo oclPlat;
    int oclCount = cv::ocl::getOpenCLPlatforms( oclPlat );

    bool ocl = oclCount>0?true:false;
    // <<<<< OPENCL available?

    return ocl;
#endif
    return false;
}


void QtOpenCVZedDemo::on_actionStart_triggered()
{
    if( !mCapture.isOpened() )
        if( !mCapture.open( ui->comboBox_cameras->currentIndex() ) )
            return;

    startTimer(0);
}

void updateFps( float time );

void QtOpenCVZedDemo::timerEvent(QTimerEvent *event)
{    
    Q_UNUSED(event);

    updateFps( mElabTime.elapsed() );
    mElabTime.restart();

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

    ui->graphicsView_left->setScene( &mLeft );

    // Show the image
    mLeft.setBgImage( left );
    mRight.setBgImage( right );

    if( ui->tabWidget->currentIndex() == 0 )
    {
        doStereoSGBM_CPU( left, right );
    }
    else if( ui->tabWidget->currentIndex() == 1 )
    {
        // TODO Cuda processing
    }
    else if( ui->tabWidget->currentIndex() == 2 )
    {
#ifndef Q_PROCESSOR_ARM
        doStereoSBM_OCL(left, right );
        //doStereoCSBP_OCL( left, right );
        //doStereoBP_OCL(left,right);
#endif
    }

    mDisp.setBgImage( mDisparity );
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

    resetFps();

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

#ifndef Q_PROCESSOR_ARM
void QtOpenCVZedDemo::doStereoSBM_OCL( cv::Mat left, cv::Mat right )
{
    cv::ocl::StereoBM_OCL sbm;

    //sbm.preset = cv::ocl::StereoBM_OCL::BASIC_PRESET;
    //sbm.ndisp = 64;
    //sbm.winSize = 9;
    sbm.preset = mOclBmWidget->getFilterMode();
    sbm.ndisp = mOclBmWidget->getDisp();
    sbm.winSize = mOclBmWidget->getWinSize();

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

void QtOpenCVZedDemo::doStereoCSBP_OCL(cv::Mat left, cv::Mat right )
{
    cv::ocl::StereoConstantSpaceBP csbp;

    int disp, iters, levels, planes;

    csbp.estimateRecommendedParams( left.cols, left.rows, disp, iters, levels, planes );

    csbp.levels = levels;
    csbp.iters = iters;
    csbp.ndisp = disp;
    csbp.nr_plane = planes;

    if( left.channels() > 1  )
        cv::cvtColor( left, left, CV_BGR2GRAY );

    if( right.channels() > 1  )
        cv::cvtColor( right, right, CV_BGR2GRAY );

    cv::ocl::oclMat ocl_left;
    cv::ocl::oclMat ocl_right;
    cv::ocl::oclMat ocl_disp;

    ocl_left.upload( left );
    ocl_right.upload( right );

    csbp( ocl_left, ocl_right, ocl_disp );

    ocl_disp.download( mDisparity );
    normalize(mDisparity, mDisparity, 0, 255, CV_MINMAX, CV_8U);
}

void QtOpenCVZedDemo::doStereoBP_OCL( cv::Mat left, cv::Mat right )
{
    cv::ocl::StereoBeliefPropagation sbp;

    int disp, iters, levels ;

    sbp.estimateRecommendedParams( left.cols, left.rows, disp, iters, levels );

    sbp.levels = levels;
    sbp.iters = iters;
    sbp.ndisp = disp;

    if( left.channels() > 1  )
        cv::cvtColor( left, left, CV_BGR2GRAY );

    if( right.channels() > 1  )
        cv::cvtColor( right, right, CV_BGR2GRAY );

    cv::ocl::oclMat ocl_left;
    cv::ocl::oclMat ocl_right;
    cv::ocl::oclMat ocl_disp;

    ocl_left.upload( left );
    ocl_right.upload( right );

    sbp( ocl_left, ocl_right, ocl_disp );

    ocl_disp.download( mDisparity );
    normalize(mDisparity, mDisparity, 0, 255, CV_MINMAX, CV_8U);
}
#endif

void QtOpenCVZedDemo::updateFps( float time )
{
    float freq = 1000.0f/time;
    float meanFps;

    if( mFpsCount < 10 )
    {

        mFpsCount++;
    }
    else
    {
        mFpsSum -= mFpsVec[mFpsIdx];

    }

    mFpsVec[mFpsIdx] = freq;
    mFpsSum += freq;
    mFpsIdx = (mFpsIdx+1)%10;
    meanFps = mFpsSum / mFpsCount;

    mFpsLabel.setText( tr("FPS: %1 hz").arg(meanFps, 5, 'f', 2) );
}

void QtOpenCVZedDemo::resetFps()
{
    mElabTime.start();

    memset( mFpsVec, 0, 10*sizeof(float) );
    mFpsCount = 0;
    mFpsIdx = 0;
    mFpsSum = 0.0f;
}

void QtOpenCVZedDemo::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index);

    resetFps();
}
