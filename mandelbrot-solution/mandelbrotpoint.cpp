#include "mandelbrotpoint.h"

#include "config.h"

#include <complex>

MandelbrotPoint::MandelbrotPoint(QPointF start_point)
    : m_start_point(start_point)
    , m_real(start_point.x())
    , m_imag(start_point.y())
{
}

double MandelbrotPoint::value(std::size_t limit)
{
    std::size_t i;
    for (i = 0; i < limit && !is_black(); ++i) {
        auto next_real = (m_real - m_imag) * (m_real + m_imag) + m_start_point.x();
        m_imag = 2 * m_real * m_imag + m_start_point.y();
        m_real = next_real;
    }
    return static_cast<double>(i % limit);
}

bool MandelbrotPoint::is_black()
{
    return m_real * m_real + m_imag * m_imag > config::constants::infinity;
}
