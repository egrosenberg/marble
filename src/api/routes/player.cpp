#include "player.h"

#define NAMESPACE api::player

API_FN_DEF(NAMESPACE, playPause) {
  ctx.activeTL->playPause();
  response["data"] = ctx.activeTL->isPlaying();
}

API_FN_DEF(NAMESPACE, skip) { ctx.activeTL->skip(); }

// options = { index: int }
API_FN_DEF(NAMESPACE, skipTo) { ctx.activeTL->skipTo((nlohmann::json::number_integer_t)options["index"]); }

API_FN_DEF(NAMESPACE, unskip) { ctx.activeTL->unskip(); }

API_FN_DEF(NAMESPACE, restart) { ctx.activeTL->restart(); }

API_FN_DEF(NAMESPACE, updateVolume) {
  response["data"] = ctx.activeTL->setVolume((nlohmann::json::number_float_t)options["level"]);
}

// options = { percent: float }
API_FN_DEF(NAMESPACE, seekPercent) { ctx.activeTL->seekPercent((nlohmann::json::number_float_t)options["percent"]); }

// options = { seconds: float }
API_FN_DEF(NAMESPACE, seekTime) { ctx.activeTL->seekTime((nlohmann::json::number_float_t)options["seconds"]); }

// options = { path: string }
API_FN_DEF(NAMESPACE, loadFile) {
  response["data"] = ctx.activeTL->loadFile((nlohmann::json::string_t)options["path"]);
}

// options = { path: string }
API_FN_DEF(NAMESPACE, appendFile) {
  response["data"] = ctx.activeTL->appendFile((nlohmann::json::string_t)options["path"]);
}

API_FN_DEF(NAMESPACE, getCurrentSong) {
  song_meta meta = ctx.activeTL->getCurrentSongMeta();
  response["data"]["name"] = meta.name;
  response["data"]["duration"] = meta.duration;
  response["data"]["startsAt"] = meta.startsAt;
  response["data"]["title"] = meta.title;
  response["data"]["artist"] = meta.artist;
  response["data"]["album"] = meta.album;
  response["data"]["year"] = meta.year;
}

API_FN_DEF(NAMESPACE, getSongNames) { response["data"] = ctx.activeTL->getSongNames(); }
