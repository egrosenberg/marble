#include "song.h"
#include "lib.h"

#include <iostream>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <chrono>

#include "spline.h"
#include <vector>

#define N_CHANNELS 2;

Song::Song(const char *fname)
{
  if (!drmp3_init_file(&m_song, fname, NULL))
  {
    std::cerr << "ERROR: Failed to open file '" << fname << "'" << std::endl;
    // throw std::exception("Unable to open audio for read");
    throw 4;
  }

  m_fname = new char[std::strlen(fname)];
  strcpy(m_fname, fname);
  m_channels = m_song.channels;
  m_sampleRate = m_song.sampleRate;
  m_sampleDuration = 1.0f / m_song.sampleRate;
  m_frames = m_song.totalPCMFrameCount;
  m_duration = 1.0f * m_sampleDuration * m_frames;
  m_volume = 1.0f;

  std::cout << "Loaded file: " << fname;
  std::cout << "\n  channels: " << m_channels;
  std::cout << "\n  sampleRate: " << m_sampleRate;
  std::cout << "\n  sampleDuration: " << m_sampleDuration;
  std::cout << "\n  frames: " << m_frames;
  std::cout << "\n  duration: " << m_duration << std::endl;
}

void Song::getFrames(float *pOutput, uint32_t frameCount)
{
  auto start = std::chrono::high_resolution_clock::now();

  uint32_t nSamples = frameCount * N_CHANNELS;
  memset(pOutput, 0, sizeof(float) * nSamples);

  float ratio = (float)m_sampleRate / TARGET_SAMPLE_RATE;
  uint32_t realFrameCount = frameCount * ratio;
  uint32_t realSampleCount = realFrameCount * N_CHANNELS;

  if (m_sampleRate == TARGET_SAMPLE_RATE)
  {
    drmp3_read_pcm_frames_f32(&m_song, realFrameCount, pOutput);
  }
  // Need to convert real audio frames into desired output frames
  else
  {
    float *realOutput = (float *)malloc(sizeof(float) * realSampleCount);
    drmp3_read_pcm_frames_f32(&m_song, realFrameCount, realOutput);

    // cubic interpolation variables
    std::vector<double> X;
    std::vector<double> YL;
    std::vector<double> YR;
    for (uint16_t i = 0, j = 0; i < realSampleCount;)
    {
      X.push_back(j++);
      YL.push_back(realOutput[i++]);
      YR.push_back(realOutput[i++]);
    }
    tk::spline leftSpline(X, YL);
    tk::spline rightSpline(X, YR);

    for (uint32_t i = 0; i < frameCount; ++i)
    {
      uint32_t left = i * 2;
      uint32_t right = i * 2 + 1;
      float sampleFrame = (float)i * ratio;
      float sampleTime = sampleFrame;

      pOutput[left] = leftSpline(sampleTime);
      pOutput[right] = rightSpline(sampleTime);

      // THIS HOPEFULLY WILL HELP INTERPOLATE FOR DIFFERING SAMPLE RATES?

      // https://en.wikipedia.org/wiki/Whittaker%E2%80%93Shannon_interpolation_formula
      // float T = m_sampleDuration;
      // float t = sampleFrame * m_sampleDuration;
      // double resL = 0;
      // double resR = 0;

      // uint32_t padding = std::min(std::min(sampleFrame, realFrameCount - sampleFrame), 200.0f);
      // uint32_t start = uint32_t(sampleFrame - padding);
      // uint32_t end = uint32_t(sampleFrame + padding);

      // for (uint32_t n = sampleFrame - padding; n <= sampleFrame + padding; ++n)
      // {
      //   float nT = n * T;
      //   resL += realOutput[n * 2] * sinc((t - nT) / T) * 0.5;
      //   resR += realOutput[n * 2 + 1] * sinc((t - nT) / T) * 0.5;
      // }

      // uint16_t x0 = std::floor(sampleFrame);
      // uint16_t x1 = std::ceil(sampleFrame);

      // pOutput[left] = lineop(x0, x1, realOutput[x0 * 2], realOutput[x0 * 2], sampleFrame);
      // pOutput[right] = lineop(x0, x1, realOutput[x0 * 2 + 1], realOutput[x0 * 2 + 1], sampleFrame);

      // pOutput[left] = resL;
      // pOutput[right] = resR;

      // std::cout << "File: " << m_fname;
      // std::cout << "\n  channels: " << m_channels;
      // std::cout << "\n  sampleRate: " << m_sampleRate << " (" << ratio << ")";
      // std::cout << "\n  sampleDuration: " << m_sampleDuration;
      // std::cout << "\n  frames: " << m_frames;
      // std::cout << "\n  duration: " << m_duration << std::endl;

      // std::cout << "sampleRate: " << m_sampleRate
      //           << "\n sampleFrame: " << sampleFrame
      //           << "\n og1: " << og1 << ", og2:" << og2
      //           << "\n diff1: " << diff1 << ", diff2: " << diff2
      //           << "\n frame1L: " << frame1L << ", frame1R: " << frame1R
      //           << "\n frame2L: " << frame2L << ", frame2R: " << frame2R
      //           << "\n pOutput[left]: " << pOutput[left] << ", pOutput[right]: " << pOutput[right]
      //           << "\n [left]: " << left << ", right: " << right
      //           << std::endl;
    }
  }

  if (m_volume != 1.0f)
  {
    for (uint32_t i = 0; i < nSamples; ++i)
    {
      pOutput[i] *= m_volume;
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "File: " << m_fname;
  // std::cout << "\n  channels: " << m_channels;
  // std::cout << "\n  sampleRate: " << m_sampleRate << " (" << ratio << ")";
  // std::cout << "\n  sampleDuration: " << m_sampleDuration;
  // std::cout << "\n  frames: " << m_frames;
  // std::cout << "\n  duration: " << m_duration;
  // std::cout << "\n  frameCount: " << frameCount << ", realFrameCount: " << realFrameCount;
  std::cout << "\n  procTime: " << duration.count();
  std::cout << std::endl;

  std::string timeStr = formatTime((float)getCurrentTime());
  std::string durationStr = formatTime((float)m_duration);
  std::cout << timeStr << '/' << durationStr << std::endl;
}

void Song::setVolume(float volume)
{
  if (volume > 1.0f)
  {
    m_volume = 1.0f;
  }
  else if (volume < 0.0f)
  {
    m_volume = 0.0f;
  }
  else
  {
    m_volume = volume;
  }
}

Song::~Song()
{
  delete m_fname;
  drmp3_uninit(&m_song);
}