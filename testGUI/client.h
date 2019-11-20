#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QHostInfo>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGuiApplication>
#include <QStyleHints>
#include <QRandomGenerator>
#include <QDebug>
#include "SendData.h"
#include "Runnable.h"

extern QVector<double> passData;
class SendData;
class Runnable;

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

    QVector<double> DataToPass() const
    {
        return passData;
    }
    void requestNewData();

private slots:
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetDataButton();
    void sessionOpened();

private:
    bool started = false;
    QComboBox *hostCombo;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    QPushButton *getDataButton;

    QTcpSocket *clientSocket;
    QDataStream in;
    QVector<double> currentData;

    QNetworkSession *networkSession = nullptr;

    SendData *sender;
    Runnable *runner;


};
#endif // CLIENT_H
