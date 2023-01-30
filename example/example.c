/* example.c -- Simple program to query the info, players and rules of a Source game server. */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
# include <winsock2.h>
#endif /* _WIN32 */

#include <ssq/a2s.h>

#define DEFAULT_PORT 27015

void ssq_info_print(const A2S_INFO *info);
void ssq_player_print(const A2S_PLAYER *players, uint8_t player_count);
void ssq_rules_print(const A2S_RULES *rules, uint16_t rule_count);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s hostname [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != NO_ERROR) {
        fprintf(stderr, "ERR: WSAStartup failed with code %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif /* _WIN32 */

    const char *hostname = argv[1];
    uint16_t port;
    if (argc == 3)
        port = atoi(argv[2]);
    else
        port = DEFAULT_PORT;

    /* Initialization */
    SSQ_SERVER server;
    ssq_server_init(&server, hostname, port);
    if (!ssq_server_ok(&server)) {
        fprintf(stderr, "ssq_server_init: %s: %s\n", hostname, ssq_server_err_msg(&server));
        exit(EXIT_FAILURE);
    }
    ssq_server_timeout(&server, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 10000);

    /* A2S_INFO */
    A2S_INFO *info = ssq_info(&server);
    if (ssq_server_ok(&server)) {
        ssq_info_print(info);
        ssq_info_free(info);
    } else {
        fprintf(stderr, "ssq_info: %s\n", ssq_server_err_msg(&server));
        ssq_server_err_clr(&server);
    }

    /* A2S_PLAYER */
    uint8_t player_count = 0;
    A2S_PLAYER *players = ssq_player(&server, &player_count);
    if (ssq_server_ok(&server)) {
        ssq_player_print(players, player_count);
        ssq_player_free(players, player_count);
    } else {
        fprintf(stderr, "ssq_player: %s\n", ssq_server_err_msg(&server));
        ssq_server_err_clr(&server);
    }

    /* A2S_RULES */
    uint16_t rule_count = 0;
    A2S_RULES *rules = ssq_rules(&server, &rule_count);
    if (ssq_server_ok(&server)) {
        ssq_rules_print(rules, rule_count);
        ssq_rules_free(rules, rule_count);
    } else {
        fprintf(stderr, "ssq_rules: %s\n", ssq_server_err_msg(&server));
        ssq_server_err_clr(&server);
    }

    /* Cleanup */
    ssq_server_cleanup(&server);

#ifdef _WIN32
    WSACleanup();
#endif /* _WIN32 */

    exit(EXIT_SUCCESS);
}

void ssq_info_print(const A2S_INFO *const info) {
    static const char *server_type_names[] = {
        [A2S_SERVER_TYPE_DEDICATED]     = "dedicated",
        [A2S_SERVER_TYPE_NON_DEDICATED] = "non-dedicated",
        [A2S_SERVER_TYPE_STV_RELAY]     = "SourceTV relay (proxy)",
        [A2S_SERVER_TYPE_UNKNOWN]       = "?",
    };
    static const char *environment_names[] = {
        [A2S_ENVIRONMENT_LINUX]   = "linux",
        [A2S_ENVIRONMENT_WINDOWS] = "windows",
        [A2S_ENVIRONMENT_MAC]     = "mac",
        [A2S_ENVIRONMENT_UNKNOWN] = "?",
    };
    printf("-----BEGIN INFO-----\n");
    printf("Protocol.......: %" PRIu8 "\n", info->protocol);
    printf("Name...........: %s\n", info->name);
    printf("Map............: %s\n", info->map);
    printf("Folder.........: %s\n", info->folder);
    printf("Game...........: %s\n", info->game);
    printf("ID.............: %" PRIu16 "\n", info->id);
    printf("Players........: %" PRIu8 "/%" PRIu8 " (%" PRIu8 " bots)\n",
        info->players, info->max_players, info->bots);
    printf("Server type....: %s\n", server_type_names[info->server_type]);
    printf("Environment....: %s\n", environment_names[info->environment]);
    printf("Visibility.....: %s\n", (info->visibility ? "private" : "public"));
    printf("VAC............: %s\n", (info->vac ? "secured" : "unsecured"));
    printf("Version........: %s\n", info->version);
    if (ssq_info_has_port(info))
        printf("Port...........: %" PRIu16 "\n", info->port);
    if (ssq_info_has_steamid(info))
        printf("SteamID........: %" PRIu64 "\n", info->steamid);
    if (ssq_info_has_stv(info)) {
        printf("Port (SourceTV): %" PRIu16 "\n", info->stv_port);
        printf("Name (SourceTV): %s\n", info->stv_name);
    }
    if (ssq_info_has_keywords(info))
        printf("Keywords.......: %s\n", info->keywords);
    if (ssq_info_has_gameid(info))
        printf("GameID.........: %" PRIu64 "\n", info->gameid);
    printf("-----END INFO-----\n");
}

void ssq_player_print(const A2S_PLAYER players[], const uint8_t player_count) {
    for (uint8_t i = 0; i < player_count; ++i) {
        printf("-----BEGIN PLAYER %" PRIu8 "-----\n", i);
        printf("Name....: %s\n", players[i].name);
        printf("Score...: %" PRId32 "\n", players[i].score);
        printf("Duration: %f\n", players[i].duration);
        printf("-----END PLAYER %" PRIu8 "-----\n", i);
    }
}

void ssq_rules_print(const A2S_RULES rules[], const uint16_t rule_count) {
    printf("-----BEGIN RULES-----\n");
    for (uint16_t i = 0; i < rule_count; ++i)
        printf("%s = %s\n", rules[i].name, rules[i].value);
    printf("-----END RULES-----\n");
}
