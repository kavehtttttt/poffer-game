#ifndef USERPANEL_H
#define USERPANEL_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>

class UserPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserPanel(QWidget *parent, QTcpSocket *socket, const QString &username);
    ~UserPanel();

private slots:
    void onPlayGameClicked(); // ارسال درخواست استارت گیم
    void handleServerMessage(); // مدیریت همه‌ی پیام‌های دریافتی (start_game, Game_Start, error)
    void onHistoryClicked(); // رفتن به صفحه تاریخچه
    void onEditInfoClicked(); // رفتن به صفحه ویرایش اطلاعات
    void onExitAccountClicked(); // خروج از حساب و برگشت به منو

private:
    void setupUI(); // ساخت و تنظیم رابط کاربری
    void updateConnectionStatus(); // به‌روزرسانی وضعیت اتصال

    QTcpSocket *socket;
    QString username;

    QWidget *centralWidget;
    QLabel *backgroundLabel;
    QLabel *connectionStatusLabel;

    QPushButton *playButton;
    QPushButton *historyButton;
    QPushButton *editInfoButton;
    QPushButton *exitButton;
};

#endif // USERPANEL_H
