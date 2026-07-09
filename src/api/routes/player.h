#ifndef API_PLAYER_H
#define API_PLAYER_H

#include "../api.h"

namespace api::player {

void playPause(const httplib::Request &, httplib::Response &, const api::context &);
inline api::fn_map postMap = {{"play-pause", &playPause}};

inline void POST(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  const api::fn_map fnMap = {{"play-pause", &playPause}};

  api::handleRequest(req, res, ctx, postMap);
}

} // namespace api::player

namespace api::handlers {
inline api::route_handlers player = {nullptr, &api::player::POST};
}

#endif