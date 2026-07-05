#ifndef SONG_H
#define SONG_H

#include "dr_mp3.h"
#include <cstdint>
#include <cstring>

#define ENDED_TOLERANCE 2000

class Song {
private:
  char *m_fname;

  drmp3 m_song;
  uint32_t m_channels;         // Number of audio channels
  uint32_t m_sampleRate;       // Sample rate of audio file
  uint32_t m_targetSampleRate; // Playback sample rate
  double
      m_sampleDuration; // Floating point duration of an audio frame in seconds
  uint64_t m_frames;    // Total number of PCM frames in file
  double m_duration;    // Duration of song in seconds
  float m_volume;       // Volume scalar (0.0 - 1.0)
public:
  Song(const char *fname, uint32_t targetSampleRate);
  virtual ~Song();

  // data callback for audio playing
  void getFrames(float *pOutput, uint32_t frameCount);

  uint64_t getCurrentFrame() { return m_song.currentPCMFrame; }
  // Get current time in seconds
  double getCurrentTime() { return m_song.currentPCMFrame * m_sampleDuration; }
  bool isEnded() {
    return (m_frames - m_song.currentPCMFrame) < ENDED_TOLERANCE;
  }
  char *getFileName() { return m_fname; };

  float getVolume() { return m_volume; }
  void setVolume(float volume);

  // Comparison
  auto operator<=>(const Song &song) { return strcmp(m_fname, song.m_fname); }
};

#endif