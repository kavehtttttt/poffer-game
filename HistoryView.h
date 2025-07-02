#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include "qjsonobject.h"
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
    explicit HistoryView(QWidget *parent = nullptr, QTcpSocket *socket = nullptr, const QString &username = "", const QJsonObject &gameHistory = QJsonObject());

private slots:
    void goBack();

private:
    void setupUI();
    void displayHistory();
    void onDataReceived();
    void sendHistoryRequest();
    QTcpSocket *socket;
    QString username;
    QJsonObject gameHistory;
    QLabel *connectionStatusLabel;
    QListWidget *historyListWidget;
    QPushButton *backButton;
};

#endif // HISTORYVIEW_H
