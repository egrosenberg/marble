#ifndef SONG_H
#define SONG_H

#include "dr_mp3.h"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>

#define ENDED_TOLERANCE 2000

typedef struct song_meta {
  std::string name;
  uint64_t startsAt;
  uint64_t duration;
} song_meta;

class Song {
private:
  char *m_fname;

  drmp3 m_song;
  std::mutex m_songMutex;
  uint32_t m_channels;         // Number of audio channels
  uint32_t m_sampleRate;       // Sample rate of audio file
  uint32_t m_targetSampleRate; // Playback sample rate
  double m_sampleDuration;     // duration of an audio frame in seconds
  uint64_t m_frames;           // Total number of PCM frames in file
  double m_duration;           // Duration of song in seconds
  float m_volume;              // Volume scalar (0.0 - 1.0)

  std::chrono::time_point<std::chrono::system_clock> *m_startsAt;

  // Fade frames are based on TARGET sample rate (process after getting frames)
  bool m_hasFadeOut;       // Flag to trigger fade out
  bool m_hasFadeIn;        // Flag to trigger fade in
  uint64_t m_fadeOutStart; // Frame to begin fade out
  uint64_t m_fadeOutEnd;   // Frame to end fade out on
  uint64_t m_fadeInStart;  // Frame to begin fade in
  uint64_t m_fadeInEnd;    // Frame to end fade in

public:
  Song(const char *fname, uint32_t targetSampleRate);
  virtual ~Song();

  // data callback for audio playing
  void getFrames(float *pOutput, uint32_t frameCount);

  double duration() { return m_duration; }

  uint64_t getCurrentFrame() { return m_song.currentPCMFrame; }
  double getCurrentTime() { return m_song.currentPCMFrame * m_sampleDuration; }
  bool isEnded() {
    return (m_frames - m_song.currentPCMFrame) < ENDED_TOLERANCE ||
           (m_hasFadeOut > 0 && m_song.currentPCMFrame > m_fadeOutEnd);
  }
  char *getFileName() { return m_fname; };
  uint64_t getFrameCount() { return m_frames; }

  song_meta getMeta();

  float getVolume() { return m_volume; }
  void setVolume(float volume);

  void seekFrame(uint64_t frame);
  void seekPercent(float percent);
  void seekTime(float);

  void setFadeIn(double start, double duration);
  void setFadeOut(double end, double duration);
  void setFadeInFrames(uint64_t start, uint64_t duration);
  void setFadeOutFrames(uint64_t end, uint64_t duration);
  void fadeOutNow(int64_t = -1);
  void removeFadeIn();
  void removeFadeOut();

  // Comparison
  auto operator<=>(const Song &song) { return strcmp(m_fname, song.m_fname); }
};

#endif