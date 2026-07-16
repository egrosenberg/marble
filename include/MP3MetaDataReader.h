#pragma once

#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

/**
 * @class MP3MetaDataReader
 * @brief A header-only C++ class for reading basic metadata from MP3 files, supporting ID3v1 and ID3v2 tags.
 *
 * This class provides functions to read MP3 metadata, such as title, artist, album, and year, from both ID3v1
 * and ID3v2 tags. It currently supports ASCII and basic UTF-16 encoded text for ID3v2 frames.
 * 
 * Usage:
 * - Call `readMetadata(filepath)` with the path to the MP3 file to retrieve metadata.
 * - The returned `MetaData` struct contains title, artist, album, and year information.
 *
 * Limitations:
 * - Only ASCII and basic UTF-16 encoded text is supported for ID3v2 tags.
 * - Currently, it does not support parsing additional metadata, such as album art, genre, comments, or track numbers.
 * 
 * Future Improvements:
 * - Add support for more encoding formats (e.g., UTF-8) for ID3v2 tags.
 * - Expand to include additional ID3 frames, such as genre, comments, and album art.
 */

class MP3MetaDataReader {
public:
    struct MetaData {
        std::string title;
        std::string artist;
        std::string album;
        std::string year;
    };

    static MetaData readMetadata(const std::string& filepath) {
        MetaData metadata;

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }
        if (hasID3v2Tag(file)) {
            readID3v2Tag(file, metadata);
        } else {
            readID3v1Tag(file, metadata);
        }

        file.close();
        return metadata;
    }

private:
    static bool hasID3v2Tag(std::ifstream& file) {
        file.seekg(0, std::ios::beg);
        char header[3];
        file.read(header, 3);
        return (header[0] == 'I' && header[1] == 'D' && header[2] == '3');
    }

    static void readID3v1Tag(std::ifstream& file, MetaData& metadata) {
        file.seekg(-128, std::ios::end);
        char buffer[128];
        file.read(buffer, 128);

        if (buffer[0] == 'T' && buffer[1] == 'A' && buffer[2] == 'G') {
            metadata.title  = std::string(buffer + 3, 30);
            metadata.artist = std::string(buffer + 33, 30);
            metadata.album  = std::string(buffer + 63, 30);
            metadata.year   = std::string(buffer + 93, 4);
        } else {
            throw std::runtime_error("No ID3v1 tag found in file.");
        }
    }

    static void readID3v2Tag(std::ifstream& file, MetaData& metadata) {
        file.seekg(0, std::ios::beg);
        char header[10];
        file.read(header, 10);

        const int tagSize = decodeSynchsafeInt(header + 6);
        int bytesRead = 10;

        while (bytesRead < tagSize) {
            char frameHeader[10];
            file.read(frameHeader, 10);
            bytesRead += 10;

            std::string frameID(frameHeader, 4);
            const int frameSize = decodeSynchsafeInt(frameHeader + 4);
            bytesRead += frameSize;

            if (frameSize > 0) {
                std::vector<char> frameContent(frameSize);
                file.read(frameContent.data(), frameSize);
                std::string content(frameContent.begin(), frameContent.end());

                if (frameID == "TIT2") {
                    metadata.title = parseTextFrame(content);
                } else if (frameID == "TPE1") {
                    metadata.artist = parseTextFrame(content);
                } else if (frameID == "TALB") {
                    metadata.album = parseTextFrame(content);
                } else if (frameID == "TYER") {
                    metadata.year = parseTextFrame(content);
                }
            }

            if (bytesRead >= tagSize) break;
        }
    }


    static int decodeSynchsafeInt(const char* buffer) {
        return ((buffer[0] & 0x7F) << 21) | ((buffer[1] & 0x7F) << 14) |
               ((buffer[2] & 0x7F) << 7) | (buffer[3] & 0x7F);
    }

    static std::string parseTextFrame(const std::string& content) {
        if (content.empty()) return "";
        const unsigned char encoding = content[0];
        std::string textContent = content.substr(1);
        if (encoding == 0) {
            return textContent;
        } else if (encoding == 1) {
            // this only handles ASCII characters in utf-16
            std::string utf16Text;
            for (size_t i = 0; i < textContent.size(); i += 2) {
                utf16Text += textContent[i + 1];
            }
            return utf16Text;
        }
        return textContent;
    }
};

