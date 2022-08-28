#pragma once

#include "calculationthread.h"
#include "config.h"

#include <QDebug>
#include <QPixmap>
#include <QWidget>

class MandelbrotWidget : public QWidget
{
    Q_OBJECT

public:
    MandelbrotWidget(QWidget * parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

public slots:
    void iterationsLimitChanged(int);
    void redIntensityChanged(int);
    void greenIntensityChanged(int);
    void blueIntensityChanged(int);
    void saveScreenshot();

private slots:
    void processRenderedImage(QImage, double scale, QPointF center);

private:
    void calculateRequest();
    QPointF getCenterOffset() const;

private:
    CalculationThread mCalculationsThread;

    QPixmap mPixmap;
    QPoint mOffset;
    QPoint mPreviousCursorPosition;
    QPointF mCenter = config::defaultValues::center;

    QString mInfo;

    double mPixmapScale = config::defaultValues::scale;
    double mScale = config::defaultValues::scale;
};
