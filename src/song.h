#ifndef SONG_H_
#define SONG_H

#include "dr_mp3.h"
#include <cstdint>

class Song
{
public:
  Song(const char *fname, uint32_t targetSampleRate);
  virtual ~Song();

  // data callback for audio playing
  void getFrames(float *pOutput, uint32_t frameCount);

  uint64_t getCurrentFrame() { return m_song.currentPCMFrame; }
  // Get current time in seconds
  double getCurrentTime() { return m_song.currentPCMFrame * m_sampleDuration; }

  float getVolume() { return m_volume; }
  void setVolume(float volume);

private:
  char *m_fname;

  drmp3 m_song;
  uint32_t m_channels;
  uint32_t m_sampleRate;
  uint32_t m_targetSampleRate;
  double m_sampleDuration;
  uint64_t m_frames;
  double m_duration;
  // 0-1.0
  float m_volume;
};

#endif