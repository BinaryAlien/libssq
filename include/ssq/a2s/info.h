#ifndef SSQ_A2S_INFO_H
#define SSQ_A2S_INFO_H

#include <stddef.h>
#include <stdint.h>

#include "ssq/server.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum ssq_a2s_info_server_type {
  SSQ_A2S_INFO_SERVER_TYPE_UNKNOWN = '\0',
  SSQ_A2S_INFO_SERVER_TYPE_DEDICATED = 'd',
  SSQ_A2S_INFO_SERVER_TYPE_NON_DEDICATED = 'l',
  SSQ_A2S_INFO_SERVER_TYPE_SOURCE_TV_RELAY = 'p',
};

enum ssq_a2s_info_environment {
  SSQ_A2S_INFO_ENVIRONMENT_UNKNOWN = '\0',
  SSQ_A2S_INFO_ENVIRONMENT_LINUX = 'l',
  SSQ_A2S_INFO_ENVIRONMENT_MAC = 'm',
  SSQ_A2S_INFO_ENVIRONMENT_WINDOWS = 'w',
};

struct ssq_a2s_info {
  /* Protocol version used by the server. */
  uint8_t protocol;

  /* Name of the server. */
  char *name;
  /* Size in bytes of the `name` UTF-8 string. */
  size_t name_size;

  /* Map the server has currently loaded. */
  char *map;
  /* Size in bytes of the `map` UTF-8 string. */
  size_t map_size;

  /* Name of the folder containing the game files. */
  char *folder;
  /* Size in bytes of the `folder` UTF-8 string. */
  size_t folder_size;

  /* Full name of the game. */
  char *game;
  /* Size in bytes of the `game` string. */
  size_t game_size;

  /* Steam Application ID of game. */
  uint16_t id;

  /* Number of players on the server. */
  uint8_t players;

  /* Maximum number of players the server reports it can hold. */
  uint8_t max_players;

  /* Number of bots on the server. */
  uint8_t bots;

  /* The type of server. */
  enum ssq_a2s_info_server_type server_type;

  /* The operating system of the server. */
  enum ssq_a2s_info_environment environment;

  /* Whether the server requires a password. */
  bool password;

  /* Whether the server uses VAC. */
  bool secure;

  /* Version of the game installed on the server. */
  char *version;

  /* Size in bytes of the `version` UTF-8 string. */
  size_t version_size;

  /* Extra Data Flag. */
  uint8_t edf;

  /* The server's game port number. */
  uint16_t port;

  /* Server's SteamID. */
  uint64_t steamid;

  /* Spectator port number for SourceTV. */
  uint16_t source_tv_port;

  /* Name of the spectator server for SourceTV. */
  char *source_tv_name;
  /* Size in bytes of the `source_tv_name` UTF-8 string. */
  size_t source_tv_name_size;

  /* Tags that describe the game according to the server. */
  char *keywords;
  /* Size in bytes of the `keywords` string. */
  size_t keywords_size;

  /* The server's 64-bit GameID. */
  uint64_t gameid;
};

struct ssq_a2s_info *ssq_a2s_info(struct ssq_server *server);
void ssq_a2s_info_free(struct ssq_a2s_info *info);

bool ssq_a2s_info_has_gameid(const struct ssq_a2s_info *info);
bool ssq_a2s_info_has_keywords(const struct ssq_a2s_info *info);
bool ssq_a2s_info_has_port(const struct ssq_a2s_info *info);
bool ssq_a2s_info_has_steamid(const struct ssq_a2s_info *info);
bool ssq_a2s_info_has_source_tv(const struct ssq_a2s_info *info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !SSQ_A2S_INFO_H */
