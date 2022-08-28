#pragma once

#include "config.h"

#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <atomic>

class CalculationThread : public QThread
{
    Q_OBJECT

public:
    CalculationThread(QObject * parent = nullptr);
    ~CalculationThread();

    void calculate(QPointF center, double scale, QSize size, double devicePixelRatio);

protected:
    void run() override;

public:
    void setIterationsLimit(int);
    void setRedIntensity(int);
    void setGreenIntensity(int);
    void setBlueIntensity(int);

signals:
    void calculated(QImage, double scale, QPointF center);

private:
    /** Optimization from Wikipedia */
    static bool isLessCardioid(const QPointF &);

private:
    QMutex mMutex;

    QWaitCondition mWaitCondition;
    QPointF mCenter;
    double mScale;
    double mDevicePixelRatio;
    QSize mSize;

    std::atomic_bool mNeedRestart = false;
    std::atomic_bool mNeedAbort = false;

    int mLimit = config::defaultValues::iterationsLimit;

    int mRedIntensity = config::defaultValues::redIntensity;
    int mGreenIntensity = config::defaultValues::greenIntensity;
    int mBlueIntensity = config::defaultValues::blueIntensity;
};
