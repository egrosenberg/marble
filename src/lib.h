#ifndef LIB_H
#define LIB_H

#include <string>
#include <cmath>

#define PI 3.141592653589793238462643383279502884
#define TARGET_SAMPLE_RATE 48000

#define MINUTES_LEN 2
#define SECONDS_LEN 2
#define MS_LEN 3

inline std::string formatTime(float seconds)
{

  std::string minutesStr = std::to_string((int)seconds / 60);
  std::string secondsStr = std::to_string((int)seconds % 60);
  secondsStr.insert(0, SECONDS_LEN - secondsStr.size(), '0');
  std::string msStr = std::to_string((int)(seconds * 1000) % 1000);
  int16_t toInsertMs = MS_LEN - msStr.size();
  if (toInsertMs > 0)
  {
    msStr.insert(0, MS_LEN - msStr.size(), '0');
  }

  std::string str = minutesStr + ":" + secondsStr + "." + msStr;
  return str;
}

inline float lineop(float x0, float x1, float y0, float y1, float x)
{
  return (y0 * (x1 - x) / (x1 - x0)) + (y1 * (x - x0) / (x1 - x0));
}

/// @brief normalized sinc
inline float sinc(float x)
{
  return std::sin(x * PI) / (x * PI);
}

#endif