#ifndef LIB_H
#define LIB_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#define TARGET_SAMPLE_RATE 48000

#define MINUTES_LEN 2
#define SECONDS_LEN 2
#define MS_LEN 3

inline std::string formatTime(float seconds) {

  std::string minutesStr = std::to_string((int)seconds / 60);
  std::string secondsStr = std::to_string((int)seconds % 60);
  secondsStr.insert(0, SECONDS_LEN - secondsStr.size(), '0');
  std::string msStr = std::to_string((int)(seconds * 1000) % 1000);
  int16_t toInsertMs = MS_LEN - msStr.size();
  if (toInsertMs > 0) {
    msStr.insert(0, MS_LEN - msStr.size(), '0');
  }

  std::string str = minutesStr + ":" + secondsStr + "." + msStr;
  return str;
}

/// @brief Reads all lines from a file and stores them in a new vector
inline std::vector<std::string> *readPlaylist(const char *fname) {
  std::vector<std::string> *res = new std::vector<std::string>();
  std::ifstream *file = new std::ifstream();
  file->open(fname);

  if (!file->is_open()) {
    printf("There was an error openeing the file %s", fname);
    return res;
  }

  std::string buffer;
  while (std::getline(*file, buffer)) {
    if (buffer.size() > 2 &&
        ((buffer.starts_with('"') && buffer.ends_with('"')) ||
         (buffer.starts_with('\'') && buffer.ends_with('\'')))) {
      buffer = buffer.substr(1, buffer.size() - 2);
    }
    res->push_back(buffer);
  }

  file->close();
  delete file;

  return res;
}

#endif