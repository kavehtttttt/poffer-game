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

private:
    QWidget *centralWidget;
    QLabel *backgroundLabel;

    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *exitButton;
    QPushButton *viewHistoryButton;
};

#endif // MAINMENU_H
