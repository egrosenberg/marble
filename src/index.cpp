
// #define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <string>
#define DR_MP3_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION

#include "api/api.h"
#include "lib.h"
#include "miniaudio.h"
#include "tracklist.h"

#include <cstdlib>
#include <getopt.h>
#include <iostream>

#define FNAME "resources/test.mp3"

uint32_t n_channels = 2;
unsigned int channel_size = 16;

Tracklist *tracklist;

uint32_t x = 0;
const double pi = std::acos(-1); // or std::numbers::pi since C++20

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
  // song->getFrames((float *)pOutput, frameCount);
  tracklist->getFrames((float *)pOutput, frameCount);
}

#define OPTION_INDEX_SAMPLE_RATE 0
#define OPTION_INDEX_LIST 1
#define OPTION_INDEX_SONG 2

int main(int argc, char **argv) {
  const char *songPath = FNAME;
  std::vector<std::string> *playlist = nullptr;

  // Getting options
  int c = 0;
  int this_option_optind = optind ? optind : 1;
  int option_index = 0;
  static struct option long_options[] = {{"sample-rate", required_argument, 0, 0},
                                         {"list", required_argument, 0, 0},
                                         {"song", required_argument, 0, 0},
                                         {0, 0, 0, 0}};

  uint32_t sampleRate = TARGET_SAMPLE_RATE;

  while (1) {
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 0:
      switch (option_index) {
      case OPTION_INDEX_SAMPLE_RATE:
        sampleRate = std::stoi(optarg);
        printf("Setting sampleRate to %ukhz\n", sampleRate);
        break;
      case OPTION_INDEX_SONG:
        songPath = optarg;
        printf("Playing song from '%s'\n", songPath);
        break;
      case OPTION_INDEX_LIST:
        playlist = readPlaylist(optarg);
        printf("Reading playlist from '%s'", optarg);
        break;
      }

      break;
    }
  }

  if (!playlist) {
    playlist = new std::vector<std::string>({songPath});
  }

  tracklist = new Tracklist(*playlist, sampleRate, 2);
  tracklist->setCutFade(6.0);

  delete playlist;

  ma_device_config config = ma_device_config_init(ma_device_type_playback);
  config.playback.format = ma_format_f32; // Set to ma_format_unknown to use the
                                          // device's native format.
  config.playback.channels = 2;           // Set to 0 to use the device's native channel count.
  config.sampleRate = sampleRate;         // Set to 0 to use the device's native sample rate.
  config.dataCallback = data_callback;    // This function will be called when
                                          // miniaudio needs more data.
  config.pUserData = NULL;                // Can be accessed from the device object (device.pUserData).

  ma_device device;
  if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
    std::cerr << "ERROR: Failed to start audio device" << std::endl;
    return -1; // Failed to initialize the device.
  }

  ma_device_start(&device); // The device is sleeping by default so you'll need
  // to start it manually.

  // Set tracklist to playing
  tracklist->play();
  // Do something here. Probably your program's main loop.

  httplib::Server svr;
  // svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
  //   // req.get_header_value(const std::string &key)
  //   std::string routeName = req.get_param_value("name");
  //   if (!routeName.length()) {
  //     printf("No api route name provided");
  //     res.set_content("No name provided", "text/plain");
  //     res.status = 400;
  //     return;
  //   }
  //   res.set_content("Hello" + routeName + "!", "text/plain");
  // });

  api::bindRoutes(svr, {tracklist});

  svr.listen("localhost", 1234);

  while (1) {
    // system("pause");
    // if (tracklist->isPaused()) {
    //   tracklist->play();
    // } else {
    //   tracklist->pause();
    // }
  }

  ma_device_uninit(&device);
  return 0;
}