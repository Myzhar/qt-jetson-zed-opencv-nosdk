#include <qopencvscene.h>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QList>

#include <opencv2/highgui/highgui.hpp>

QOpenCVScene::QOpenCVScene(QObject *parent) :
    QGraphicsScene(parent),
    mBgPixmapItem(NULL)
{
    setBackgroundBrush( QBrush(QColor(200,200,200)));

    mJoypadBgItem = new QGraphicsPixmapItem( QPixmap(":/joypad/images/joystick_background.png") );
    mJoypadBgItem->setOffset( -50, -50);

    mJoypadThumbItem = new QGraphicsPixmapItem( QPixmap(":/joypad/images/joystick_thumb.png") );
    mJoypadThumbItem->setOffset( -25, -25 );

    addItem(mJoypadBgItem);
    addItem(mJoypadThumbItem);

    mJoypadBgItem->setVisible(false);
    mJoypadThumbItem->setVisible(false);

    mJoypadBgItem->setZValue(1);
    mJoypadThumbItem->setZValue(2);
}

void QOpenCVScene::setBgImage( cv::Mat& cvImg )
{
    if(!mBgPixmapItem)
    {
        mBgPixmapItem = new QGraphicsPixmapItem( cvMatToQPixmap(cvImg) );
        //cv::imshow( "Test", cvImg );
        mBgPixmapItem->setPos( 0,0 );

        mBgPixmapItem->setZValue( 0.0 );
        addItem( mBgPixmapItem );
    }
    else
        mBgPixmapItem->setPixmap( cvMatToQPixmap(cvImg) );

    setSceneRect( 0,0, cvImg.cols, cvImg.rows );
}

void QOpenCVScene::setJoypadSize( QSize bgSize, QSize thumbSize )
{    
    double origW = mJoypadBgItem->pixmap().width();

    double scale = bgSize.width()/origW;

    mJoypadBgItem->setOffset(-bgSize.width()/(2*scale),-bgSize.height()/(2*scale));
    mJoypadBgItem->setScale(scale);

    mJoypadThumbItem->setOffset(-thumbSize.width()/(2*scale),-thumbSize.height()/(2*scale));
    mJoypadThumbItem->setScale(scale);
}

void QOpenCVScene::buttonDown( QPointF mBnDownPos )
{       
    mJoypadBgItem->setPos(mBnDownPos);
    mJoypadThumbItem->setPos(mBnDownPos);

    mJoypadBgItem->setVisible(true);
    mJoypadThumbItem->setVisible(true);
}

void QOpenCVScene::buttonUp( )
{
    mJoypadBgItem->setVisible(false);
    mJoypadThumbItem->setVisible(false);
}

void QOpenCVScene::mouseMove(QPointF mMousePos )
{
    mJoypadThumbItem->setPos(mMousePos);
}


QImage QOpenCVScene::cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
    // 8-bit, 4 channel
    case CV_8UC4:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

        return image;
    }

        // 8-bit, 3 channel
    case CV_8UC3:
    {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

        return image.rgbSwapped();
    }

        // 8-bit, 1 channel
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;

        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }

        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

        image.setColorTable( sColorTable );

        return image;
    }

    default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }

    return QImage();
}

QPixmap QOpenCVScene::cvMatToQPixmap( const cv::Mat &inMat )
{
    return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

