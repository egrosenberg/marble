#ifndef TRACK_H
#define TRACK_H

#include <string>
#include <vector>

#include "song.h"

class Tracklist {
private:
  std::vector<std::string> *m_fnames; // list of paths to songs

  // Trying not to use this
  // Do we ever need more than two active songs?
  std::vector<Song *> *m_activeSongs; // All currently opened songs

  uint32_t m_sampleRate;    // Sample rate to play songs at
  uint16_t m_channels;      // Number of channels to output
  uint16_t m_cursor;        // Index of current song in list
  int32_t m_mixCursor;      // Index of mixed song
  Song *m_currentSong;      // Pointer to song object of current song
  Song *m_mixedSong;        // Pointer to song object of secondary song (for fades)
  bool m_paused;            // Is tracklist paused?
  uint32_t m_fadeOutFrames; // Duration of song fade out
  uint32_t m_fadeInFrames;  // Duration of song fade in
  uint32_t m_fadeInDelay;   // Delay (frames) between fade out start and fade in start
  bool m_fadingOut;         // Is song actively fading out
  bool m_fadingIn;          // Is song actively fading in
  int64_t m_fadeInAt;       // Frame to start fade in (-1 = not scheduled)

  void loadSong(uint16_t cursor);

public:
  Tracklist(const std::vector<std::string> &fnames, uint32_t sampleRate, uint16_t channels);
  virtual ~Tracklist();

  void getFrames(float *pOutput, uint32_t frameCount);

  uint16_t length() { return m_fnames->size(); }

  bool isPlaying() { return !m_paused; }
  bool isPaused() { return m_paused; }

  void play() { m_paused = false; }
  void pause() { m_paused = true; }

  void setCrossfade(double duration); // Equal cross fade
  void setCutFade(double duration);   // Fade out, cut in halfway through fade out
  void setFadeOutduration(double duration);
  void setFadeInDelay(double duration);
  void setFadeInDuration(double duration);
};

#endif