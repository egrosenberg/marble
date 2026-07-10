#ifndef API_PLAYER_H
#define API_PLAYER_H

#include "../api.h"

namespace api::player {

void playPause(API_FN_ARGS);
void skip(API_FN_ARGS);
void unskip(API_FN_ARGS);
void restart(API_FN_ARGS);
void updateVolume(API_FN_ARGS);

inline const api::fn_map postMap = {{"play-pause", &playPause},
                                    {"unskip", &unskip},
                                    {"skip", &skip},
                                    {"restart", &restart},
                                    {"update-volume", &updateVolume}};

inline void POST(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  const api::fn_map fnMap = {{"play-pause", &playPause}};

  api::handleRequest(req, res, ctx, postMap);
}

} // namespace api::player

namespace api::handlers {
inline api::route_handlers player = {nullptr, &api::player::POST};
}

#endif