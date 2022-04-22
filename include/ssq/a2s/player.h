#ifndef SSQ_A2S_PLAYER_H
#define SSQ_A2S_PLAYER_H

#include "ssq/ssq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct a2s_player {
    uint8_t index;    /** Index of player chunk starting from 0                     */
    char   *name;     /** Name of the player                                        */
    size_t  name_len; /** Length of the `name' string                               */
    int32_t score;    /** Player's score (usually "frags" or "kills")               */
    float   duration; /** Time (in seconds) player has been connected to the server */
} A2S_PLAYER;

/**
 * Sends an A2S_PLAYER query to a Source game server.
 *
 * @param querier      Source server querier to use
 * @param player_count where to store the number of players in the output array
 *
 * @return dynamically-allocated array of `A2S_PLAYER' structs containing details about each player
 *         on the server, or NULL if an error occurred or there are no players connected
 */
A2S_PLAYER *ssq_player(SSQ_QUERIER *querier, uint8_t *player_count);

/**
 * Frees an `A2S_PLAYER' array.
 *
 * @param players      `A2S_PLAYER' array to free
 * @param player_count number of players in the `A2S_PLAYER' array
 */
void ssq_player_free(A2S_PLAYER *players, uint8_t player_count);

#ifdef __cplusplus
}
#endif

#endif /* SSQ_A2S_PLAYER_H */
