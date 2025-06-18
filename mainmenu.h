#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

private slots:
    void openSignup();
    void openLogin();

private:
    QWidget *centralWidget;
    QLabel *backgroundLabel;

    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *exitButton;
};

#endif // MAINMENU_H
