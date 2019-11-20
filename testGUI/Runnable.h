#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QRandomGenerator>
#include <QtWidgets>
#include "client.h"

class Client;
class Runnable : public QRunnable
{
    QVector<double> mData;
    QObject *mReciever;
    bool mRunning;
    QRandomGenerator rand;
public:
    Runnable(QObject *receiver)
    {
        mReciever = receiver;
        mRunning = false;
    }
    void run()
    {
        mRunning = true;
        while(mRunning)
        {
            qDebug() << "really";
            mData.clear();
            //mData << rand.bounded(0, 360)
                  //<< rand.bounded(0, 12000);
            QMetaObject::invokeMethod(mReciever, "setNumber",
                                      Qt::QueuedConnection,
                                      Q_ARG(QVector<double>, mData));
            QThread::msleep(100);
        }
    }
    bool isRunning() const
    {
        return mRunning;
    }
    void stop()
    {
        mRunning = false;
    }

    void setMData(QVector<double> newStuff)
    {
        qDebug() << "wtf";
        mData = newStuff;
    }
};

#endif // RUNNABLE_H
