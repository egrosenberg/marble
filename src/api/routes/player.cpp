#include "player.h"

void api::player::playPause(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  ctx.activeTL->playPause();
}

void api::player::skip(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  ctx.activeTL->skip();
}

void api::player::unskip(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  ctx.activeTL->unskip();
}

void api::player::restart(const httplib::Request &req, httplib::Response &res, const api::context &ctx) {
  ctx.activeTL->restart();
}