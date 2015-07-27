QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-jetson-zed-opencv-nosdk
TEMPLATE = app

INCLUDEPATH+=./cqtopencvviewergl

SOURCES += main.cpp\
           qt-jetson-zed-opencv-nosdk.cpp \
           cqtopencvviewergl/cqtopencvviewergl.cpp \
    qoclbm_params.cpp

HEADERS  += qt-jetson-zed-opencv-nosdk.h \
            cqtopencvviewergl/cqtopencvviewergl.h \
    qoclbm_params.h

FORMS    += qt-jetson-zed-opencv-nosdk.ui \
    qoclbm_params.ui

win32 {
message("Using win32 configuration")

OPENCV_PATH = C:/devel/OpenCV/opencv-2.4.6.0 # Note: update with the correct OpenCV version

#LIBS_PATH = "$$OPENCV_PATH/build/x86/mingw/lib" #project compiled using MINGW
LIBS_PATH = "$$OPENCV_PATH/build/x86/vc10/lib" #project compiled using Visual C++ 2010 32bit compiler

    CONFIG(debug, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core246d \
                -lopencv_highgui246d
    }

    CONFIG(release, debug|release) {
    LIBS     += -L$$LIBS_PATH \
                -lopencv_core246 \
                -lopencv_highgui246
    }
}

unix {
message("Using unix configuration")

OPENCV_PATH = /usr/opencv2

LIBS_PATH = /usr/lib

LIBS     += \
    -L$$LIBS_PATH \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_gpu \
    -lopencv_ocl \
    -lopencv_imgproc \
    -lopencv_calib3d
}

INCLUDEPATH += \
    $$OPENCV_PATH/modules/core/include/ \ #core module
    $$OPENCV_PATH/modules/highgui/include/ #highgui modul

message("OpenCV path: $$OPENCV_PATH")
message("Includes path: $$INCLUDEPATH")
message("Libraries: $$LIBS")


