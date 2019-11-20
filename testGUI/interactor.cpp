#include "interactor.h"

Interactor::Interactor(QObject *parent) : QObject(parent)
{
    changeSomethingSpeed();
}

void Interactor::changeSomethingSpeed()
{
    speed = rand.bounded(0, 360);
    m_data = speed;
    setData(m_data);
}

int Interactor::data()
{
    return m_data;
}

void Interactor::setData(int &data)
{
    if (data != m_data)
    {
        data = m_data;
        changeSomethingSpeed();
    }
    emit dataChanged();
}
