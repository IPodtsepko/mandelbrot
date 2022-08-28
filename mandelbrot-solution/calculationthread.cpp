#include "calculationthread.h"

#include "mandelbrotpoint.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QImage>
#include <QTextStream>
#include <cmath>
#include <complex>
#include <thread>
#include <vector>

namespace details {

QPointF
toPointF(const QSize & size)
{
    return QPointF(size.width(), size.height());
}

QPoint
toPoint(const QSize & size)
{
    return QPoint(size.width(), size.height());
}

} // namespace details

CalculationThread::CalculationThread(QObject * parent)
    : QThread(parent)
{
}

CalculationThread::~CalculationThread()
{
    {
        QMutexLocker locker(&mMutex);
        mNeedAbort = true;
        mWaitCondition.wakeOne();
    }
    wait();
}

void CalculationThread::calculate(QPointF center, double scale, QSize size, double devicePixelRatio)
{
    QMutexLocker locker(&mMutex);

    mCenter = center;
    mScale = scale;
    mDevicePixelRatio = devicePixelRatio;
    mSize = size;

    if (isRunning()) {
        mNeedRestart = true;
        mWaitCondition.wakeOne();
    }
    else {
        start(LowPriority);
    }
}

void CalculationThread::run()
{
    QElapsedTimer timer;
    forever
    {
        mMutex.lock();
        const auto devicePixelRatio = mDevicePixelRatio;
        const auto size = mSize * devicePixelRatio;
        const auto requestedScale = mScale;
        const auto scale = devicePixelRatio / requestedScale;
        const auto center = mCenter;
        const auto limit = mLimit;
        const auto requestedColor = QColor(mRedIntensity, mGreenIntensity, mBlueIntensity);
        mMutex.unlock();

        QPoint screenSize = details::toPoint(size) / 3;
        QRect screen(screenSize.x(), screenSize.y(), screenSize.x(), screenSize.y());
        const auto pixmapCenter = details::toPointF(size) / 2;

        const auto getColor = [&](int x, int y) {
            const auto offset = QPointF(x, y) - pixmapCenter;
            MandelbrotPoint pixel(center + offset * scale);
            auto shader = (isLessCardioid(offset) ? 0 : pixel.value(limit)) / limit;
            return qRgb(requestedColor.red() * shader, requestedColor.green() * shader, requestedColor.blue() * shader);
        };

        QImage image(size, QImage::Format_RGB32);
        image.setDevicePixelRatio(devicePixelRatio);

        auto data = reinterpret_cast<uint *>(image.bits());

        timer.restart();

        const auto partCount = QThread::idealThreadCount() * 3;
        std::atomic_int currentPart;

        const auto partiallyRender = [&](const int pixelSize) {
            int scaledHeight = std::ceil(1.0 * size.height() / pixelSize);
            forever
            {
                int part = currentPart++;
                if (part > partCount) {
                    return;
                }
                auto stripBegin = part * scaledHeight / partCount * pixelSize;
                auto stripEnd = std::min((part + 1) * scaledHeight / partCount * pixelSize, size.height());
                for (auto i = stripBegin; i < stripEnd && !mNeedRestart; i += pixelSize) {
                    if (mNeedAbort) {
                        return;
                    }
                    for (auto j = 0; j < size.width() && !mNeedRestart && !mNeedAbort; j += pixelSize) {
                        auto color = getColor(j, i);
                        auto scanline = i * size.width();

                        auto leftBound = std::min(i + pixelSize, size.height());
                        auto bottomBound = std::min(j + pixelSize, size.width());

                        for (auto y = i; y < leftBound; ++y, scanline += size.width()) {
                            for (auto x = j; x < bottomBound; ++x) {
                                data[scanline + x] = screen.contains(x, y) ? getColor(x, y) : color;
                            }
                        }
                    }
                }
            }
        };

        for (int pixelScale = config::constants::lowResolutionPixelSize; pixelScale > 0; pixelScale /= 4) {
            currentPart = 0;
            const auto threadCount = QThread::idealThreadCount();
            std::vector<std::thread> threads(threadCount);
            for (int i = 0; i < threadCount; ++i) {
                threads[i] = std::thread(partiallyRender, pixelScale);
            }
            for (int i = 0; i < threadCount; ++i) {
                threads[i].join();
            }

            QString message;
            QTextStream stream(&message);
            stream.setRealNumberPrecision(2);
            stream << "Thread count: " << threadCount
                   << ", time: " << timer.elapsed() / 1000.0 << " s"
                   << ", scale: " << requestedScale / config::defaultValues::scale;
            image.setText(config::constants::infoKey, message);
            emit calculated(image.copy(), requestedScale, center);
        }

        {
            QMutexLocker locker(&mMutex);
            if (mNeedAbort) {
                return;
            }
            if (!mNeedRestart) {
                mWaitCondition.wait(&mMutex);
            }
            mNeedRestart = false;
        }
    }
}

void CalculationThread::setIterationsLimit(int v)
{
    QMutexLocker locker(&mMutex);
    mLimit = v;
}

void CalculationThread::setRedIntensity(int v)
{
    QMutexLocker locker(&mMutex);
    mRedIntensity = v;
}

void CalculationThread::setGreenIntensity(int v)
{
    QMutexLocker locker(&mMutex);
    mGreenIntensity = v;
}

void CalculationThread::setBlueIntensity(int v)
{
    QMutexLocker locker(&mMutex);
    mBlueIntensity = v;
}

bool CalculationThread::isLessCardioid(const QPointF & point)
{
    std::complex<double> shifted(point.x() - 0.25, point.y());
    return std::abs(shifted) < 0.5 * (1 - cos(atan2(shifted.imag(), shifted.real())));
}
