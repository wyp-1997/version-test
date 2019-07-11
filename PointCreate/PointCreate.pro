TEMPLATE = app
CONFIG += console c++11
QMAKE_CXXFLAGS += -std=c++0x
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH+=/usr/include/eigen3

INCLUDEPATH+=/usr/include/vtk-6.2
LIBS+=/usr/lib/x86_64-linux-gnu/libvtk*.so

INCLUDEPATH+=/usr/include/boost
LIBS+=/usr/lib/x86_64-linux-gnu/libboost_*.so

INCLUDEPATH+=/usr/include/pcl-1.7
LIBS+=/usr/lib/x86_64-linux-gnu/libpcl_*.so

INCLUDEPATH += /usr/include/ni
LIBS += -L/usr/lib -lOpenNI

INCLUDEPATH += /home/wyp/PointCreate/PointCreate/include


INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv \
               /usr/local/include/opencv2

LIBS += /usr/local/lib/libopencv_highgui.so \
        /usr/local/lib/libopencv_core.so    \
        /usr/local/lib/libopencv_imgproc.so \
        /usr/local/lib/libopencv_imgcodecs.so  \
        /usr/local/lib/libopencv_*.so

INCLUDEPATH += -I /opt/ros/kinetic/include

DEPENDPATH +=  /opt/ros/kinetic/include

LIBS += -L /opt/ros/kinetic/lib/ -lroscpp
LIBS += -L /opt/ros/kinetic/lib/ -lroslib
LIBS += -L /opt/ros/kinetic/lib/ -lpthread
LIBS += -L /opt/ros/kinetic/lib/ -lroscpp_serialization
LIBS += -L /opt/ros/kinetic/lib/ -lrostime
LIBS += -L /opt/ros/kinetic/lib/ -lrosconsole
LIBS += -L /opt/ros/kinetic/lib/ -lrosconsole_log4cxx
LIBS += -L /opt/ros/kinetic/lib/ -lrosconsole_backend_interface
LIBS += -L /opt/ros/kinetic/lib/ -lxmlrpcpp
LIBS += /opt/ros/kinetic/lib/lib*.so

SOURCES += \
    main.cpp \
    source/CameraParam.cpp \
    source/DepthProcess.cpp \
    source/EdgeDetection.cpp \
    source/RemoveHand.cpp \
    source/RgbImageProcess.cpp

HEADERS += \
    include/CameraParam.h \
    include/RgbSubscribe.h \
    include/DepthSubscribe.h \
    include/DepthProcess.h \
    include/EdgeDetection.h \
    include/RgbImageProcess.h \
    include/RemoveHand.h





