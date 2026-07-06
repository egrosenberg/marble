#include "tracklist.h"
#include "song.h"

#include <iostream>

Tracklist::Tracklist(const std::vector<std::string> &fnames, uint32_t sampleRate, uint16_t channels) {
  m_sampleRate = sampleRate;
  m_channels = channels;
  m_cursor = 0;
  m_mixCursor = -1;
  m_paused = true;

  m_fnames = new std::vector<std::string>(fnames);
  m_activeSongs = new std::vector<Song *>();

  // Hard coded for now
  m_fadeOutFrames = 0;
  m_fadeInFrames = 0;
  m_fadeInDelay = 0;
  m_fadeInAt = -1;

  m_fadingOut = false;
  m_fadingIn = false;

  m_currentSong = nullptr;
  m_mixedSong = nullptr;

  // Load first song
  loadSong(0);
}

void Tracklist::loadSong(uint16_t cursor) {
  if (!m_fnames->size()) {
    std::cerr << "No songs to load" << std::endl;
    return;
  }

  m_fadingOut = false;

  std::printf("Loading song at %u\n", cursor);

  // Keep cursor within bounds
  cursor = cursor % m_fnames->size();
  if (cursor < 0) {
    cursor += m_fnames->size();
  }

  if (m_currentSong) {
    delete m_currentSong;
  }
  if (m_mixCursor == cursor) {
    m_currentSong = m_mixedSong;
    m_mixedSong = nullptr;
  } else {
    m_currentSong = new Song(m_fnames->at(cursor).c_str(), m_sampleRate);
  }
}

void Tracklist::getFrames(float *pOutput, uint32_t frameCount) {

  if (m_paused) {
    std::memset(pOutput, 0, frameCount * m_channels * sizeof(float));
    return;
  }
  // Ensure we have a song if we are playing
  if (!m_currentSong) {
    loadSong(m_cursor);
  }
  // get output frames from song
  m_currentSong->getFrames(pOutput, frameCount);

  uint64_t currentFrame = m_currentSong->getCurrentFrame();

  // Check if song is ready to fade out
  if (!m_fadingOut && m_fadeOutFrames && (currentFrame + m_fadeOutFrames) > m_currentSong->getFrameCount()) {
    std::cout << "Fading out at " << currentFrame << std::endl;
    m_fadingOut = true;
    m_fadeInAt = currentFrame + m_fadeInDelay;
    m_currentSong->fadeOutNow();
  }
  if (m_fadeInAt != -1 && currentFrame > m_fadeInAt && !m_fadingIn && !m_mixedSong) {
    m_fadingIn = true;
    m_mixCursor = (m_cursor + 1) % m_fnames->size();
    m_mixedSong = new Song(m_fnames->at(m_mixCursor).c_str(), m_sampleRate);
    m_mixedSong->setFadeInFrames(0, m_fadeInFrames);
  }
  if (m_mixedSong) {
    float *mixFrames = (float *)malloc(sizeof(float) * frameCount * m_channels);
    m_mixedSong->getFrames(mixFrames, frameCount);

    for (uint32_t i = 0; i < frameCount * m_channels; ++i) {
      pOutput[i] += mixFrames[i];
    }

    free(mixFrames);
  }

  // Check if song is ended
  if (m_currentSong->isEnded()) {
    m_fadingOut = false;
    if (!m_mixedSong) {
      loadSong(++m_cursor);
    } else {
      m_fadeInAt = -1;
      delete m_currentSong;
      m_currentSong = m_mixedSong;
      m_mixedSong = nullptr;
      m_mixCursor = -1;
    }
  }
}

/// @brief Equal crossfade
/// @param duration: duration of fade in seconds
void Tracklist::setCrossfade(double duration) {
  uint64_t frames = duration * m_sampleRate;

  m_fadeOutFrames = frames;
  m_fadeInFrames = frames;
  m_fadeInDelay = 0;
}

/// @brief Fade out, cut in halfway through fade out
/// @param duration: duration of fade out
void Tracklist::setCutFade(double duration) {
  uint64_t frames = duration * m_sampleRate;

  m_fadeOutFrames = frames;
  m_fadeInFrames = frames * 0.5f;
  m_fadeInDelay = m_fadeInFrames;
}

/// @brief Set fade out duration in seconds
void Tracklist::setFadeOutduration(double duration) { m_fadeOutFrames = m_sampleRate * duration; }

/// @brief Set fade in delay in seconds (0 for crossfade)
void Tracklist::setFadeInDelay(double duration) { m_fadeInDelay = m_sampleRate * duration; }

/// @brief set fade in duration in seconds
void Tracklist::setFadeInDuration(double duration) { m_fadeInFrames = m_sampleRate * duration; }

Tracklist::~Tracklist() {
  delete m_fnames;
  delete m_activeSongs;

  if (m_currentSong)
    delete m_currentSong;
  if (m_mixedSong)
    delete m_mixedSong;
}