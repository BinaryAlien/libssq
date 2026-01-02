#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#endif /* _WIN32 */

#include <ssq/a2s.h>

static void ssq_a2s_info_print(const struct ssq_a2s_info *info);
static void ssq_a2s_player_print(const struct ssq_a2s_player *players,
                                 uint8_t count);
static void ssq_a2s_rules_print(const struct ssq_a2s_rule *rules,
                                uint16_t count);

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s <HOST> <PORT>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

#ifdef _WIN32
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR) {
    fprintf(stderr, "ERR: WSAStartup failed with code %d\n", WSAGetLastError());
    exit(EXIT_FAILURE);
  }
#endif /* _WIN32 */

  const char *host = argv[1];
  uint16_t port = atoi(argv[2]);

  /* Initialization */
  struct ssq_server *server = ssq_server_from_address(host, port);
  if (server == NULL) {
    fprintf(stderr, "ssq_server_new: memory exhausted\n");
    exit(EXIT_FAILURE);
  } else if (!ssq_server_eok(server)) {
    fprintf(stderr, "ssq_server_new: %s: %s\n", host,
            ssq_server_emessage(server));
    ssq_server_free(server);
    exit(EXIT_FAILURE);
  }
  ssq_server_timeout(server, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 3000);

  /* A2S_INFO */
  struct ssq_a2s_info *info = ssq_a2s_info(server);
  if (ssq_server_eok(server)) {
    ssq_a2s_info_print(info);
    ssq_a2s_info_free(info);
  } else {
    fprintf(stderr, "ssq_a2s_info: %s\n", ssq_server_emessage(server));
    ssq_server_eclear(server);
  }

  /* A2S_PLAYER */
  uint8_t player_count = 0;
  struct ssq_a2s_player *players = ssq_a2s_player(server, &player_count);
  if (ssq_server_eok(server)) {
    ssq_a2s_player_print(players, player_count);
    ssq_a2s_player_free(players, player_count);
  } else {
    fprintf(stderr, "ssq_a2s_player: %s\n", ssq_server_emessage(server));
    ssq_server_eclear(server);
  }

  /* A2S_RULES */
  uint16_t rule_count = 0;
  struct ssq_a2s_rule *rules = ssq_a2s_rules(server, &rule_count);
  if (ssq_server_eok(server)) {
    ssq_a2s_rules_print(rules, rule_count);
    ssq_a2s_rules_free(rules, rule_count);
  } else {
    fprintf(stderr, "ssq_a2s_rules: %s\n", ssq_server_emessage(server));
    ssq_server_eclear(server);
  }

  /* Cleanup */
  ssq_server_free(server);

#ifdef _WIN32
  WSACleanup();
#endif /* _WIN32 */

  exit(EXIT_SUCCESS);
}

static void ssq_a2s_info_print(const struct ssq_a2s_info *info) {
  static const char *server_type_names[] = {
      [SSQ_A2S_INFO_SERVER_TYPE_DEDICATED] = "dedicated",
      [SSQ_A2S_INFO_SERVER_TYPE_NON_DEDICATED] = "non-dedicated",
      [SSQ_A2S_INFO_SERVER_TYPE_SOURCE_TV_RELAY] = "SourceTV relay (proxy)",
      [SSQ_A2S_INFO_SERVER_TYPE_UNKNOWN] = "?",
  };
  static const char *environment_names[] = {
      [SSQ_A2S_INFO_ENVIRONMENT_LINUX] = "linux",
      [SSQ_A2S_INFO_ENVIRONMENT_WINDOWS] = "windows",
      [SSQ_A2S_INFO_ENVIRONMENT_MAC] = "mac",
      [SSQ_A2S_INFO_ENVIRONMENT_UNKNOWN] = "?",
  };
  printf("A2S_INFO:\n");
  printf("Protocol..........: %" PRIu8 "\n", info->protocol);
  printf("Name..............: %s\n", info->name);
  printf("Map...............: %s\n", info->map);
  printf("Folder............: %s\n", info->folder);
  printf("Game..............: %s\n", info->game);
  printf("ID................: %" PRIu16 "\n", info->id);
  printf("Players...........: %" PRIu8 "/%" PRIu8 " (%" PRIu8 " bots)\n",
         info->players, info->max_players, info->bots);
  printf("Server type.......: %s\n", server_type_names[info->server_type]);
  printf("Environment.......: %s\n", environment_names[info->environment]);
  printf("Password..........: %s\n", info->password ? "yes" : "no");
  printf("Secure............: %s\n", info->secure ? "yes" : "no");
  printf("Version...........: %s\n", info->version);
  if (ssq_a2s_info_has_port(info))
    printf("Port..............: %" PRIu16 "\n", info->port);
  if (ssq_a2s_info_has_steamid(info))
    printf("SteamID...........: %" PRIu64 "\n", info->steamid);
  if (ssq_a2s_info_has_source_tv(info)) {
    printf("Port (SourceTV)...: %" PRIu16 "\n", info->source_tv_port);
    printf("Name (SourceTV)...: %s\n", info->source_tv_name);
  }
  if (ssq_a2s_info_has_keywords(info))
    printf("Keywords..........: %s\n", info->keywords);
  if (ssq_a2s_info_has_gameid(info))
    printf("GameID............: %" PRIu64 "\n", info->gameid);
  printf("\n");
}

static void ssq_a2s_player_print(const struct ssq_a2s_player *players,
                                 uint8_t count) {
  for (uint8_t index = 0; index < count; ++index) {
    printf("A2S_PLAYER %" PRIu8 ":\n", index);
    printf("Index......: %" PRIu8 "\n", players[index].index);
    printf("Name.......: %s\n", players[index].name);
    printf("Score......: %" PRId32 "\n", players[index].score);
    printf("Duration...: %d min %02d\n", (int)players[index].duration / 60,
           (int)players[index].duration % 60);
    printf("\n");
  }
}

static void ssq_a2s_rules_print(const struct ssq_a2s_rule *rules,
                                uint16_t count) {
  printf("A2S_RULES:\n");
  for (uint16_t index = 0; index < count; ++index)
    printf("%s = %s\n", rules[index].name, rules[index].value);
  printf("\n");
}
