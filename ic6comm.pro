QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    comm_worker.cpp \
      main.cpp \
    ic6_comm.cpp \
    thirdparty/ipaddress.cpp \
    utility/circle_widget.cpp \
    utility/write_data.cpp

HEADERS += \
    comm_worker.h \
       stable.h \
    ic6_comm.h \
    thirdparty/ipaddress.h \
    utility/circle_widget.h \
    utility/write_data.h

FORMS += \
    ic6_comm.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

PRECOMPILED_HEADER = stable.h

QMAKE_CXXFLAGS += /MP

RC_ICONS = logo.ico

RESOURCES += \
    facade/res.qrc

win32 {
    CONFIG(debug, debug|release) {  # 需要基于 Debug 模式
        DEFINES += VLD_MODULE
        VLD_PATH = "C:\Program Files (x86)\Visual Leak Detector"
        INCLUDEPATH += $${VLD_PATH}/include
        LIBS += -L$${VLD_PATH}/lib/Win32 -lvld
    }
}

