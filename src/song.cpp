#include "song.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <vector>

#include "MP3MetaDataReader.h"
#include "spline.h"

#include "lib/math.h"

#define N_CHANNELS 2;

Song::Song(const char *fname, uint32_t targetSampleRate) {
  m_targetSampleRate = targetSampleRate;

  if (!drmp3_init_file(&m_song, fname, nullptr)) {
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

  m_startsAt = nullptr;

  m_hasFadeOut = false;
  m_fadeOutStart = 0;
  m_fadeOutEnd = 0;

  m_hasFadeIn = false;
  m_fadeInStart = 0;
  m_fadeInEnd = 0;

  // Read metadata
  m_fileMeta = MP3MetaDataReader::readMetadata(fname);

  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  // std::time_t t_now = std::chrono::system_clock::to_time_t(now);
  std::time_t t_end = std::chrono::system_clock::to_time_t(now + std::chrono::seconds((uint64_t)m_duration));

  std::cout << "Loaded file: " << fname;
  std::cout << "\n  channels: " << m_channels;
  std::cout << "\n  sampleRate: " << m_sampleRate;
  // std::cout << "\n  sampleDuration: " << m_sampleDuration;
  std::cout << "\n  targetSampleRate: " << m_targetSampleRate;
  std::cout << "\n  frames: " << m_frames;
  std::cout << "\n  duration: " << m_duration;
  std::cout << "\n  ends at: " << std::ctime(&t_end) << std::endl;
}

void Song::getFrames(float *pOutput, uint32_t frameCount) {
  if (m_song.currentPCMFrame == 0) {
    m_startsAt = new std::chrono::time_point<std::chrono::system_clock>(std::chrono::system_clock::now());
  }

  // std::chrono::time_point<std::chrono::high_resolution_clock> start =
  // std::chrono::high_resolution_clock::now();

  uint64_t startFrame = m_song.currentPCMFrame;
  uint64_t endFrame = m_song.currentPCMFrame + frameCount;

  uint32_t nSamples = frameCount * m_channels;

  if (m_sampleRate == m_targetSampleRate) {
    m_songMutex.lock();
    drmp3_read_pcm_frames_f32(&m_song, frameCount, pOutput);
    m_songMutex.unlock();
  }
  // Need to convert real audio frames into desired output frames
  else {
    memset(pOutput, 0, sizeof(float) * nSamples);

    float ratio = (float)m_sampleRate / m_targetSampleRate;
    uint32_t realFrameCount = frameCount * ratio;
    uint32_t realSampleCount = realFrameCount * m_channels;

    // Store actual output frames in a buffer
    float *realOutput = (float *)malloc(sizeof(float) * realSampleCount);
    m_songMutex.lock();
    drmp3_read_pcm_frames_f32(&m_song, realFrameCount, realOutput);
    m_songMutex.unlock();

    // construct splines for each channel
    std::vector<double> X;
    std::vector<double> YL;
    std::vector<double> YR;
    for (uint16_t i = 0, j = 0; i < realSampleCount;) {
      // Don't count frames past end of track
      if (i + m_song.currentPCMFrame < m_frames) {
        X.push_back(j++);
        YL.push_back(realOutput[i++]);
        YR.push_back(realOutput[i++]);
      }
    }
    tk::spline leftSpline(X, YL);
    tk::spline rightSpline(X, YR);

    for (uint32_t i = 0; i < frameCount; ++i) {
      float sampleFrame = (float)i * ratio;
      // Don't process frames past end of track
      if ((std::ceil(sampleFrame) + m_song.currentPCMFrame) < m_frames) {
        pOutput[i * 2] = leftSpline(sampleFrame);
        pOutput[i * 2 + 1] = rightSpline(sampleFrame);
      } else {
        pOutput[i * 2] = 0;
        pOutput[i * 2 + 1] = 0;
      }
    }
  }

  if (m_volume != 1.0f) {
    for (uint32_t i = 0; i < nSamples; ++i) {
      pOutput[i] *= m_volume;
    }
  }

  // Process fade
  if (m_hasFadeIn) {
    for (uint32_t i = 0; i < frameCount; ++i) {
      uint32_t frame = i + startFrame;
      if (frame > m_fadeInEnd)
        break;

      uint32_t l = i * 2;
      uint32_t r = i * 2 + 1;

      float x = HALF_PI * (((float)frame - m_fadeInStart) / (m_fadeInEnd - m_fadeInStart));
      float value = approx_sin2(x);

      pOutput[i * 2] *= value;
      pOutput[i * 2 + 1] *= value;
    }
  }
  if (m_hasFadeOut) {
    for (uint32_t i = 0; i < frameCount; ++i) {
      uint32_t frame = i + startFrame;
      if (frame > m_fadeOutEnd) {
        pOutput[i * 2] = 0;
        pOutput[i * 2 + 1] = 0;
        continue;
      }
      uint32_t l = i * 2;
      uint32_t r = i * 2 + 1;

      float value = approx_cos2(HALF_PI * ((float)(frame - m_fadeOutStart) / (m_fadeOutEnd - m_fadeOutStart)));

      pOutput[l] *= value;
      pOutput[r] *= value;
    }
  }

  // std::string timeStr = formatTime((float)getCurrentTime());
  // std::string durationStr = formatTime((float)m_duration);
  // std::cout << m_fname << ": " << timeStr << '/' << durationStr << std::endl;

  // std::chrono::time_point<std::chrono::high_resolution_clock> end =
  // std::chrono::high_resolution_clock::now(); std::chrono::microseconds
  // duration = std::chrono::duration_cast<std::chrono::microseconds>(end -
  // start);

  // std::cout << "Computed " << frameCount << " frames in " << duration <<
  // std::endl;
}

void Song::setVolume(float volume) {
  if (volume > 1.0f) {
    m_volume = 1.0f;
  } else if (volume < 0.0f) {
    m_volume = 0.0f;
  } else {
    m_volume = volume;
  }
}

void Song::seekFrame(uint64_t frame) {
  if (!m_song.pSeekPoints) {
    uint32_t seekPointCount = m_frames;
    drmp3_calculate_seek_points(&m_song, &seekPointCount, m_song.pSeekPoints);
    std::cout << "Calculated " << seekPointCount << " seek points\n";
  }
  frame = std::min(m_song.totalPCMFrameCount, frame);

  std::cout << "Seeking frame " << frame << " of " << m_frames << std::endl;
  m_songMutex.lock();
  drmp3_seek_to_pcm_frame(&m_song, frame);
  m_songMutex.unlock();
  std::cout << "Current frame: " << m_song.currentPCMFrame << std::endl;
}

void Song::seekTime(float seconds) {
  uint64_t frame = m_sampleRate * seconds;
  seekFrame(frame);
}

void Song::seekPercent(float percent) {
  percent = std::min(1.0f, std::max(0.0f, percent));
  seekFrame(std::floor(percent * m_frames));
}

/// @brief Set fade in
/// @param start:     start time of fade (in seconds)
/// @param duration:  duration of fade (in seconds)
void Song::setFadeIn(double start, double duration) {
  m_hasFadeIn = true;
  m_fadeInStart = start * m_targetSampleRate;
  m_fadeInEnd = m_fadeInStart + (duration * m_targetSampleRate);
}

/// @brief Set fade in
/// @param start:     start time of fade (in seconds)
/// @param duration:  duration of fade (in seconds)
void Song::setFadeInFrames(uint64_t start, uint64_t duration) {
  m_hasFadeIn = true;
  m_fadeInStart = start;
  m_fadeInEnd = std::min(m_fadeInStart + duration, m_frames);
}

/// @brief Set fade out
/// @param end:       end time of fade out (seconds)
/// @param duration:  duratoin of fade out (seconds)
void Song::setFadeOut(double end, double duration) {
  m_hasFadeOut = true;
  m_fadeOutEnd = end * m_targetSampleRate;
  m_fadeOutStart = std::max(0.0, m_fadeOutEnd - (m_targetSampleRate * duration));
}

/// @brief Set fade out
/// @param end:       end time of fade out (PCM frames)
/// @param duration:  duration of fade out (PCM frames)
void Song::setFadeOutFrames(uint64_t end, uint64_t duration) {
  m_hasFadeOut = true;
  m_fadeOutEnd = std::min(m_frames, end);
  m_fadeOutStart = std::max(0llu, m_fadeOutEnd - duration);
}

/// @brief fades out song immediately
/// @param duration: optional duration in frames to fade out over (default is to fade until end of song)
void Song::fadeOutNow(int64_t duration) {
  m_hasFadeOut = true;
  m_fadeOutEnd = duration < 0 ? m_frames : m_song.currentPCMFrame + duration;
  m_fadeOutStart = m_song.currentPCMFrame;
}
/// @brief remove fade in
void Song::removeFadeIn() { m_hasFadeIn = false; }
/// @brief remove fade out
void Song::removeFadeOut() { m_hasFadeOut = false; }

song_meta Song::getMeta() {
  song_meta meta;
  meta.duration = m_frames / m_sampleRate * 1000;

  std::filesystem::path fpath(m_fname);
  meta.name = fpath.stem().generic_string();

  meta.startsAt =
      m_startsAt ? std::chrono::duration_cast<std::chrono::milliseconds>(m_startsAt->time_since_epoch()).count() : -1;

  meta.title = m_fileMeta.title;
  meta.artist = m_fileMeta.artist;
  meta.album = m_fileMeta.album;
  meta.year = m_fileMeta.year;

  return meta;
}

Song::~Song() {
  delete m_fname;
  drmp3_uninit(&m_song);

  if (m_startsAt)
    delete m_startsAt;
}