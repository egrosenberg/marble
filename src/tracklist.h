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

  uint32_t m_sampleRate;   // Sample rate to play songs at
  uint16_t m_channels;     // Number of channels to output
  uint16_t m_cursor;       // Index of current song in list
  int32_t m_mixCursor;     // Index of mixed song
  Song *m_currentSong;     // Pointer to song object of current song
  Song *m_mixedSong;       // Pointer to song object of secondary song (for fades)
  bool m_paused;           // Is tracklist paused?
  uint32_t m_fadeDuration; // Duration of crossfade
  int64_t m_fadeAt;        // Frame to start crossfade (-1 if never)

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
};

#endif