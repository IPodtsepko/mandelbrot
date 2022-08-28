#pragma once

#include <QPoint>
#include <QString>
#include <complex>

namespace config {

namespace defaultValues {

static constexpr auto center = QPointF(-0.7, 0);
static constexpr auto scale = 200.0;

// Performance
static constexpr auto iterationsLimit = 64;

// Colors
static constexpr auto redIntensity = 0x9A;
static constexpr auto greenIntensity = 0xCD;
static constexpr auto blueIntensity = 0x32;

} // namespace defaultValues

namespace constants {

// Rendering
static constexpr auto infinity = 4;
static constexpr auto lowResolutionPixelSize = 4;

// Zoom
static constexpr auto zoomInFactor = 1.25;
static constexpr auto zoomOutFactor = 1 / zoomInFactor;
static constexpr auto wheelStepsDivisor = 120.0;

inline const auto infoKey = QStringLiteral("info");

} // namespace constants

} // namespace config
