#include "mandelbrotwidget.h"

#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QPainter>
#include <math.h>

namespace details {

QPointF toPointF(const QSize &);

} // namespace details

MandelbrotWidget::MandelbrotWidget(QWidget * parent)
    : QWidget(parent)
{
    connect(&mCalculationsThread, &CalculationThread::calculated, this, &MandelbrotWidget::processRenderedImage);
}

// Events handlers

void MandelbrotWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (mPixmap.isNull()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("Please wait for the end of the calculations"));
        return;
    }

    if (mScale == mPixmapScale) {
        painter.drawPixmap(mOffset - details::toPointF(size()), mPixmap);
    }
    else {
        const auto pixmap = mPixmap.scaled(mPixmap.size() / mPixmap.devicePixelRatio());

        painter.save();

        painter.translate(mOffset - details::toPointF(size()));

        const auto factor = mScale / mPixmapScale;
        painter.scale(factor, factor);

        painter.drawPixmap(QPoint(), pixmap);
        painter.restore();
    }

    if (mInfo.isEmpty()) {
        return;
    }

    QFontMetrics metrics = painter.fontMetrics();
    int textWidth = metrics.horizontalAdvance(mInfo);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(16, 16, 16, 192));
    painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
    painter.setPen(Qt::white);
    painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), mInfo);
}

void MandelbrotWidget::resizeEvent(QResizeEvent *)
{
    calculateRequest();
}

void MandelbrotWidget::wheelEvent(QWheelEvent * event)
{
    const auto stepsCount = event->angleDelta().y() / 120.0;
    const auto factor = pow(config::constants::zoomInFactor, stepsCount);

    mOffset += (factor - 1) * (mOffset - QPoint(width(), height()) - event->position().toPoint());
    mScale *= factor;

    calculateRequest();
}

void MandelbrotWidget::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        mPreviousCursorPosition = event->position().toPoint();
    }
}

void MandelbrotWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton) {
        mOffset += event->position().toPoint() - mPreviousCursorPosition;
        mPreviousCursorPosition = event->position().toPoint();
        update();
    }
}

void MandelbrotWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        mOffset += event->position().toPoint() - mPreviousCursorPosition;
        mPreviousCursorPosition = QPoint();
        calculateRequest();
    }
}

// Spinbox handlers

void MandelbrotWidget::iterationsLimitChanged(int v)
{
    mCalculationsThread.setIterationsLimit(v);
    calculateRequest();
}

void MandelbrotWidget::redIntensityChanged(int v)
{
    mCalculationsThread.setRedIntensity(v);
    calculateRequest();
}

void MandelbrotWidget::greenIntensityChanged(int v)
{
    mCalculationsThread.setGreenIntensity(v);
    calculateRequest();
}

void MandelbrotWidget::blueIntensityChanged(int v)
{
    mCalculationsThread.setBlueIntensity(v);
    calculateRequest();
}

void MandelbrotWidget::saveScreenshot()
{
    QString name = "screenshot";

    if (!QFile::exists(name + ".jpg")) {
        mPixmap.save("screenshot.jpg");
        return;
    }

    std::size_t i = 1;
    while (QFile::exists("screenshot (" + QString::number(i) + ").jpg")) {
        ++i;
    }
    mPixmap.save("screenshot (" + QString::number(i) + ").jpg");
}

// Slots

void MandelbrotWidget::processRenderedImage(QImage image, double scaleFactor, QPointF center)
{
    if (!mPreviousCursorPosition.isNull() ||
        scaleFactor != mScale ||
        center != mCenter + getCenterOffset()) {
        return;
    }

    mInfo = image.text(config::constants::infoKey);

    mPixmap = QPixmap::fromImage(image);
    mCenter = center;
    mOffset = QPoint();
    mPreviousCursorPosition = QPoint();
    mPixmapScale = scaleFactor;

    update();
}

// Private

void MandelbrotWidget::calculateRequest()
{
    update();
    mCalculationsThread.calculate(mCenter + getCenterOffset(), mScale, 3 * size(), devicePixelRatio());
}

QPointF MandelbrotWidget::getCenterOffset() const
{
    const auto center = details::toPointF(mPixmap.size()) / (2 * mPixmap.devicePixelRatio());
    return (center * (1 - mScale / mPixmapScale) - mOffset) / mScale;
}
