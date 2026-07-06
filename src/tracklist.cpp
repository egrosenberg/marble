#include "tracklist.h"
#include "song.h"

#include <iostream>

Tracklist::Tracklist(const std::vector<std::string> &fnames,
                     uint32_t sampleRate, uint16_t channels) {
  m_sampleRate = sampleRate;
  m_channels = channels;
  m_cursor = 0;
  m_mixCursor = -1;
  m_paused = true;

  m_fnames = new std::vector<std::string>(fnames);
  m_activeSongs = new std::vector<Song *>();

  // Hard coded for now
  m_fadeDuration = sampleRate * 6;
  m_fadeAt = -1;

  m_currentSong = nullptr;
  m_mixedSong = nullptr;

  // Load first song
  loadSong(0);

  std::cout << "fadeDuration" << m_fadeDuration << std::endl;
}

void Tracklist::loadSong(uint16_t cursor) {
  if (!m_fnames->size()) {
    std::cerr << "No songs to load" << std::endl;
    return;
  }

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

  // Check if song is ready to fade
  if (m_fadeAt == -1 &&
      (currentFrame + m_fadeDuration) > m_currentSong->getFrameCount()) {
    std::cout << "Fading out at " << currentFrame << std::endl;
    m_fadeAt = currentFrame + m_fadeDuration / 2;
    m_currentSong->fadeOutNow();
  }
  if (currentFrame > m_fadeAt && !m_mixedSong) {
    m_mixCursor = (m_cursor + 1) % m_fnames->size();
    m_mixedSong = new Song(m_fnames->at(m_mixCursor).c_str(), m_sampleRate);
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
    if (!m_mixedSong) {
      loadSong(++m_cursor);
    } else {
      m_fadeAt = -1;
      delete m_currentSong;
      m_currentSong = m_mixedSong;
      m_mixedSong = nullptr;
      m_mixCursor = -1;
    }
  }
}

Tracklist::~Tracklist() {
  delete m_fnames;
  delete m_activeSongs;

  if (m_currentSong)
    delete m_currentSong;
  if (m_mixedSong)
    delete m_mixedSong;
}