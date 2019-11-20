#ifndef INTERACTOR_H
#define INTERACTOR_H

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QObject>
#include <QDebug>
#include <QQmlContext>
#include <QRandomGenerator>

class Interactor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit Interactor(QObject *parent = nullptr);
    Q_INVOKABLE int getSpeed() {
        changeSomethingSpeed();
        return speed;}
    Q_INVOKABLE void changeSpeed() {speed++;}

signals:
    void dataChanged();

public slots:

private:
    void changeSomethingSpeed();
    int data();
    void setData(int &data);
    int speed;
    int previousSpeed = 0;
    QRandomGenerator rand;
    int m_data;
};

#endif // INTERACTOR_H
