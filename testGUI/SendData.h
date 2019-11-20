#ifndef SENDDATA_H
#define SENDDATA_H

#include "Runnable.h"

#include <QObject>
#include <QThreadPool>
#include <QDebug>

class SendData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector<double> number READ number WRITE setNumber NOTIFY numberChanged)
public:
    explicit SendData(QObject *parent = nullptr):QObject(parent){
        m_number << 0
                 << 0;
        runnable = new Runnable(this);
    }
    ~SendData(){
        runnable->stop();
    }
    Q_INVOKABLE void start(){
        if(!runnable->isRunning())
            QThreadPool::globalInstance()->start(runnable);
    }
    QVector<double> number() const{
        return m_number;
    }
public slots:
    void setNumber(QVector<double> number){
        if(number == m_number)
            return;
        m_number = number;
        emit numberChanged(m_number);
    }
signals:
    void numberChanged(QVector<double>);
private:
    QVector<double> m_number;
    Runnable *runnable;
};
#endif // SENDDATA_H
