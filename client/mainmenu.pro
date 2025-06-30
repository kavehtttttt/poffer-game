QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CardRevealManager.cpp \
    CardWidget.cpp \
    EditInfo.cpp \
    GameBoard.cpp \
    HistoryView.cpp \
    UserPanel.cpp \
    ValidationException.cpp \
    WaitingRoom.cpp \
    login.cpp \
    main.cpp \
    mainmenu.cpp \
    resetpassword.cpp \
    signup.cpp

HEADERS += \
    CardRevealManager.h \
    CardWidget.h \
    EditInfo.h \
    GameBoard.h \
    HistoryView.h \
    UserPanel.h \
    ValidationException.h \
    WaitingRoom.h \
    login.h \
    mainmenu.h \
    resetpassword.h \
    signup.h

FORMS += \
    UserPanel.ui \
    basewindow.ui \
    cardrevealmanager.ui \
    cardwidget.ui \
    editinfo.ui \
    gameboard.ui \
    gameboardwindow.ui \
    gamewindow.ui \
    graphiccard.ui \
    historyview.ui \
    login.ui \
    mainmenu.ui \
    resetpassword.ui \
    signup.ui \
    waitingroom.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
