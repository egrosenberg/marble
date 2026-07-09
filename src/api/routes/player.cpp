#include "player.h"

void api::player::playPause(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  ctx.activeTL->playPause();
}