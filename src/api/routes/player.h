#ifndef API_PLAYER_H
#define API_PLAYER_H

#include "../api.h"

namespace api::player {

// POST functions
API_FN(playPause)
API_FN(skip)
API_FN(unskip)
API_FN(restart)
API_FN(updateVolume)
API_FN(seekPercent)
API_FN(seekTime)

// GET functions
API_FN(getCurrentSong)
API_FN(getSongNames)

inline const api::fn_map postMap = {{"play-pause", &playPause},
                                    {"unskip", &unskip},
                                    {"skip", &skip},
                                    {"restart", &restart},
                                    {"update-volume", &updateVolume},
                                    {"seek-percent", &seekPercent},
                                    {"seek-time", &seekTime}};

inline const api::fn_map getMap = {{"get-current-song", &getCurrentSong}, {"get-song-names", &getSongNames}};

POST_HANDLER
GET_HANDLER

} // namespace api::player

namespace api::handlers {
BIND_ROUTE(player);
}

#endif