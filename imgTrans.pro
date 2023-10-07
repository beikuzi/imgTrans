QT       += core gui
INCLUDEPATH += D:/env/build-opencv-4.8.0-Desktop_Qt_6_5_2_MinGW_64_bit-Release/install/include \
D:/env/build-opencv-4.8.0-Desktop_Qt_6_5_2_MinGW_64_bit-Release/install/include/opencv2
LIBS += -LD:/env/build-opencv-4.8.0-Desktop_Qt_6_5_2_MinGW_64_bit-Release/install/x64/mingw/bin \
D:/env/build-opencv-4.8.0-Desktop_Qt_6_5_2_MinGW_64_bit-Release/install/x64/mingw/lib/libopencv_*

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#LIBS += -lqzxing
SOURCES += \
    bkzOpencv.cpp \
    main.cpp \
    widget.cpp

HEADERS += \
    bkzAlgorithms.h \
    bkzOpencv.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RC_ICONS=1.ico
