#pragma once

#include <QPointF>

class MandelbrotPoint
{
public:
    MandelbrotPoint(QPointF);

    double value(std::size_t limit);

private:
    bool is_black();

private:
    QPointF m_start_point;

    double m_real;
    double m_imag;
};
