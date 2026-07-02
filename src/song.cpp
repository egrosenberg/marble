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
    // Store actual output frames in a buffer
    float *realOutput = (float *)malloc(sizeof(float) * realSampleCount);
    drmp3_read_pcm_frames_f32(&m_song, realFrameCount, realOutput);

    // construct splines for each channel
    std::vector<double> X;
    std::vector<double> YL;
    std::vector<double> YR;
    for (uint16_t i = 0, j = 0; i < realSampleCount;)
    {
      // Don't count frames past end of track
      if (i + getCurrentFrame() < m_frames)
      {
        X.push_back(j++);
        YL.push_back(realOutput[i++]);
        YR.push_back(realOutput[i++]);
      }
    }
    tk::spline leftSpline(X, YL);
    tk::spline rightSpline(X, YR);

    for (uint32_t i = 0; i < frameCount; ++i)
    {
      float sampleFrame = (float)i * ratio;
      // Zero out frames past end of track
      if (std::ceil(sampleFrame) + getCurrentFrame() >= m_frames)
      {
        pOutput[i * 2] = 0;
        pOutput[i * 2 + 1] = 0;
      }
      else
      {
        pOutput[i * 2] = leftSpline(sampleFrame);
        pOutput[i * 2 + 1] = rightSpline(sampleFrame);
      }
    }
  }

  if (m_volume != 1.0f)
  {
    for (uint32_t i = 0; i < nSamples; ++i)
    {
      pOutput[i] *= m_volume;
    }
  }

  std::string timeStr = formatTime((float)getCurrentTime());
  std::string durationStr = formatTime((float)m_duration);
  std::cout << m_fname << ": " << timeStr << '/' << durationStr << std::endl;
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