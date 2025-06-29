#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QWidget>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>

class HistoryView : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryView(QWidget *parent = nullptr, QTcpSocket *socket = nullptr, const QString &username = "");

private slots:
    void goBack();
    void onDataReceived();

private:
    void sendHistoryRequest();
    void setupUI();

    QTcpSocket *socket;
    QString username;

    QLabel *connectionStatusLabel;
    QListWidget *historyListWidget;
    QPushButton *backButton;
};

#endif // HISTORYVIEW_H
