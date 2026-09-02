QT       += core gui charts xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


include(main/main.pri)
include(tt_tools/tt_tools.pri)
include(login/login.pri)
include(t_QCustomPlot/tt_qcustomplot.pri)
include(log_system/log_system.pri)
include(tem_ModbusTcp/tt_temModbusTcp.pri)
include(NtcPtc/ntcptc.pri)

INCLUDEPATH += $$PWD/tt_tools
INCLUDEPATH += $$PWD/t_QCustomPlot
INCLUDEPATH += $$PWD/main
INCLUDEPATH += $$PWD/login
INCLUDEPATH += $$PWD/log_system
INCLUDEPATH += $$PWD/tem_ModbusTcp
INCLUDEPATH += $$PWD/NtcPtc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    search.qrc

win32: LIBS += -L$$PWD/ACS/ -lACSCL_x64

INCLUDEPATH += $$PWD/ACS
DEPENDPATH += $$PWD/ACS
