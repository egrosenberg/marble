#ifndef LIB_MATH_H
#define LIB_MATH_H

#include <cmath>

#define PI 3.141592653589793238462643383279502884f
#define HALF_PI 1.570796326794897f
#define PI_MAJOR PI
#define PI_MINOR -0.00000008742278f

/// @brief calculates linear interpolation between two points
inline float lineop(float x0, float x1, float y0, float y1, float x) {
  return (y0 * (x1 - x) / (x1 - x0)) + (y1 * (x - x0) / (x1 - x0));
}

// The following two functions are based on the work details by Colin Wallace at
// https://web.archive.org/web/20200628195036/http://mooooo.ooo/chebyshev-sine-approximation/
// And the c++ implementation and updates at
// https://www.apulsoft.ch/blog/branchless-sincos/

// could use std::fma if faster - depends on cpu/compiler configuration
inline float _fma(float a, float b, float c) { return a * b + c; }

// Approximate sin(x) and cos(x) between -pi and pi
// relative err |f(x)/sin(x) - 1|
// sin x: 1.32e-6 near 0
// cos x: 2.07e-6 at +-2.99

/// @brief approximatino of sine using chebyshev polynomials
inline float approx_sin(float x) {
  constexpr float s0 = -0.10132104963779f;     // x
  constexpr float s1 = 0.00662060857089096f;   // x^3
  constexpr float s2 = -0.000173351320734045f; // x^5
  constexpr float s3 = 2.48668816803878e-06f;  // x^7
  constexpr float s4 = -1.97103310997063e-08f; // x^9

  constexpr float c0 = -0.405284410277645f;    // 1
  constexpr float c1 = 0.0383849982168558f;    // x^2
  constexpr float c2 = -0.00132798793179218f;  // x^4
  constexpr float c3 = 2.37446117208029e-05f;  // x^6
  constexpr float c4 = -2.23984068352572e-07f; // x^8

  auto x2 = x * x;

  // evaluate two 4th-order polynomials of (x^2) using estrin's scheme.
  auto x4 = x2 * x2;
  auto x8 = x4 * x4;
  auto poly1 = _fma(x8, s4, _fma(x4, _fma(s3, x2, s2), _fma(s1, x2, s0)));

  return (x - PI) * (x + PI) * x * poly1;
}

/// @brief approximatino of cosine using chebyshev polynomials
inline float approx_cos(float x) {
  constexpr float s0 = -0.10132104963779f;     // x
  constexpr float s1 = 0.00662060857089096f;   // x^3
  constexpr float s2 = -0.000173351320734045f; // x^5
  constexpr float s3 = 2.48668816803878e-06f;  // x^7
  constexpr float s4 = -1.97103310997063e-08f; // x^9

  constexpr float c0 = -0.405284410277645f;    // 1
  constexpr float c1 = 0.0383849982168558f;    // x^2
  constexpr float c2 = -0.00132798793179218f;  // x^4
  constexpr float c3 = 2.37446117208029e-05f;  // x^6
  constexpr float c4 = -2.23984068352572e-07f; // x^8

  auto x2 = x * x;

  // evaluate two 4th-order polynomials of (x^2) using estrin's scheme.
  auto x4 = x2 * x2;
  auto x8 = x4 * x4;
  auto poly2 = _fma(x8, c4, _fma(x4, _fma(c3, x2, c2), _fma(c1, x2, c0)));

  return (x - HALF_PI) * (x + HALF_PI) * poly2;
}

/// END REFERENCE

/// @brief sine squared (approximated for speed)
inline float approx_sin2(float x) {
  return (1.0f - approx_cos(2.0f * x)) * 0.5f;
}

/// @brief cosine squared (approximated for speed)
inline float approx_cos2(float x) {
  return (1.0f + approx_cos(2.0f * x)) * 0.5f;
}
/// @brief tangent squared (approximated for speed)
inline float approx_tan2(float x) {
  return (1.0f - approx_cos(2.0f * x)) / (1.0f + approx_cos(2.0f * x));
}

/// @brief sine squared
inline float sin2(float x) { return (1.0f - std::cos(2.0f * x)) * 0.5f; }

/// @brief cosine squared
inline float cos2(float x) { return (1.0f + std::cos(2.0f * x)) * 0.5f; }

/// @brief tangent squared
inline float tan2(float x) {
  return (1.0f - std::cos(2.0f * x)) / (1.0f + approx_cos(2.0f * x));
}

#endif