#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTcpSocket>

class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    virtual ~BaseWindow();

protected:
    virtual void setupUI() = 0; // هر کلاس مشتق‌شده باید UI خود را بسازد
    void setupConnectionStatus();
    void setupSocketSignals();

    QTcpSocket *socket;
    QLabel *connectionStatusLabel;
};

#endif // BASEWINDOW_H
