
#define DR_MP3_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION

#include "lib.h"
#include "dr_mp3.h"
#include "miniaudio.h"
#include "song.h"

#include <iostream>
#include <cmath>
#include <bitset>

#define FNAME "resources/test.mp3"
#define FNAME_2 "resources/test2.mp3"

drmp3_uint32 n_channels = 2;
unsigned int channel_size = 16;

Song *song;
Song *song2;

uint32_t x = 0;
const double pi = std::acos(-1); // or std::numbers::pi since C++20

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
  uint32_t nSamples = frameCount * 2;

  song->getFrames((float *)pOutput, frameCount);

  float *song2Frames = (float *)malloc(sizeof(float) * nSamples);
  song2->getFrames(song2Frames, frameCount);

  // Copy data into channels
  float *pOutputFrames = (float *)pOutput;

  for (uint16_t i = 0; i < nSamples; ++i)
  {
    pOutputFrames[i] += song2Frames[i];
    // channels[i] = 0;
    x++;
  }
}

int main()
{
  song = new Song(FNAME);

  song2 = new Song(FNAME_2);

  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.format = ma_format_f32; // Set to ma_format_unknown to use the device's native format.
  config.playback.channels = 2;           // Set to 0 to use the device's native channel count.
  config.sampleRate = TARGET_SAMPLE_RATE; // Set to 0 to use the device's native sample rate.
  config.dataCallback = data_callback;    // This function will be called when miniaudio needs more data.
  config.pUserData = NULL;                // Can be accessed from the device object (device.pUserData).

  ma_device device;
  if (ma_device_init(NULL, &config, &device) != MA_SUCCESS)
  {
    std::cerr << "ERROR: Failed to start audio device" << std::endl;
    return -1; // Failed to initialize the device.
  }

  ma_device_start(&device); // The device is sleeping by default so you'll need to start it manually.

  // Do something here. Probably your program's main loop.
  while (1)
  {
  }

  ma_device_uninit(&device);
  return 0;
}