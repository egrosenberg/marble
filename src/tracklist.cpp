#include "tracklist.h"

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

  // Load first song
  loadSong(0);
}

void Tracklist::loadSong(uint16_t cursor) {
  if (!m_fnames->size()) {
    std::cerr << "No songs to load" << std::endl;
    return;
  }

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

  // Check if song is ended
  loadSong(++m_cursor);
}

Tracklist::~Tracklist() {
  delete m_fnames;
  delete m_activeSongs;

  if (m_currentSong)
    delete m_currentSong;
  if (m_mixedSong)
    delete m_mixedSong;
}