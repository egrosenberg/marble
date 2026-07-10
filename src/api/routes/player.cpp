#include "player.h"

void api::player::playPause(const nlohmann::json &options, nlohmann::json &response, const api::context &ctx) {
  ctx.activeTL->playPause();
  response["data"] = ctx.activeTL->isPlaying();
}

void api::player::skip(const nlohmann::json &options, nlohmann::json &response, const api::context &ctx) {
  ctx.activeTL->skip();
}

void api::player::unskip(const nlohmann::json &options, nlohmann::json &response, const api::context &ctx) {
  ctx.activeTL->unskip();
}

void api::player::restart(const nlohmann::json &options, nlohmann::json &response, const api::context &ctx) {
  ctx.activeTL->restart();
}

void api::player::updateVolume(const nlohmann::json &options, nlohmann::json &response, const api::context &ctx) {
  response["data"] = ctx.activeTL->setVolume((nlohmann::json::number_float_t)options["level"]);
}