#ifndef SSQ_A2S_PLAYER_H
#define SSQ_A2S_PLAYER_H

#include <stddef.h>

#include "ssq/server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ssq_a2s_player {
  /* Index of player chunk starting from 0. */
  uint8_t index;

  /* Name of the player. */
  char *name;
  /* Size in bytes of the `name` UTF-8 string. */
  size_t name_size;

  /* Player's score (usually "frags" or "kills"). */
  int32_t score;

  /* Time (in seconds) player has been connected to the server. */
  float duration;
};

struct ssq_a2s_player *ssq_a2s_player(struct ssq_server *server,
                                      uint8_t *count);

void ssq_a2s_player_free(struct ssq_a2s_player *players, uint8_t count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_A2S_PLAYER_H */
