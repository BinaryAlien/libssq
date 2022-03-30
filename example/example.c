/*
 * example.c
 *
 * This example program takes a Source game server hostname and optional port number as an
 * argument and sends it the 3 following A2S queries in order: A2S_INFO, A2S_PLAYER, A2S_RULES.
 * Each of the responses from these queries are pretty-printed to the standard output.
 */

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
# include <winsock2.h>
#endif

#include <ssq/a2s.h>

#define DEFAULT_PORT 27015

/* Pretty-prints an A2S_INFO struct */
void ssq_info_print(const A2S_INFO *info);

/* Pretty-prints an A2S_PLAYER array */
void ssq_player_print(const A2S_PLAYER *players, uint8_t player_count);

/* Pretty-prints an A2S_RULES array */
void ssq_rules_print(const A2S_RULES *rules, uint16_t rule_count);

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s hostname [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "ERR: WSAStartup failed with code %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
#endif

    const char *hostname = argv[1];
    uint16_t    port;

    if (argc == 3)
        port = atoi(argv[2]);
    else
        port = DEFAULT_PORT;

    /* Initialization */

    // Initialize a new heap-allocated Source server querier
    SSQ_QUERIER *querier = ssq_init();

    // Assert that the memory allocation was successful
    assert(querier != NULL);

    /* Set the timeout for both receiving and sending operations to 3000 ms.
     * Note: If this function is *not* called, the timeouts for receiving/sending operations will be set to the default
     * values of `SSQ_TIMEOUT_RECV_DEFAULT_VALUE` and `SSQ_TIMEOUT_SEND_DEFAULT_VALUE` respectively */
    ssq_set_timeout(querier, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 3000);

    // Set the Source server querier's target
    ssq_set_target(querier, hostname, port);

    // Error handling
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_set_target: %s: %s\n", hostname, error_message);

        // IMPORTANT: Always clear the error once we are done handling it
        ssq_errclr(querier);

        exit(EXIT_FAILURE);
    }


    /* A2S_INFO */

    // Send an A2S_INFO query
    A2S_INFO *info = ssq_info(querier);

    // Error handling
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_info: %s\n", error_message);

        // IMPORTANT: Always clear the querier's error once we are done handling it
        ssq_errclr(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-print the A2S_INFO query's response
    ssq_info_print(info);

    // IMPORTANT: Always free the A2S_INFO struct once we are done using it
    ssq_info_free(info);


    /* A2S_PLAYER */

    // Send an A2S_PLAYER query
    uint8_t player_count = 0;
    A2S_PLAYER *players = ssq_player(querier, &player_count);

    // Error handling
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_player: %s\n", error_message);

        // IMPORTANT: Always clear the querier's error once we are done handling it
        ssq_errclr(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-print the A2S_PLAYER query's response
    ssq_player_print(players, player_count);

    // IMPORTANT: Always free the A2S_PLAYER array once we are done using it
    ssq_player_free(players, player_count);


    /* A2S_RULES */

    // Send an A2S_RULES query
    uint16_t rule_count = 0;
    A2S_RULES *rules = ssq_rules(querier, &rule_count);

    // Error handling
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_rules: %s\n", error_message);

        // IMPORTANT: Always clear the querier's error once we are done handling it
        ssq_errclr(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-print the A2S_RULES query's response
    ssq_rules_print(rules, rule_count);

    // IMPORTANT: Always free the A2S_RULES array once we are done using it
    ssq_rules_free(rules, rule_count);


    // IMPORTANT: Always free the Source server querier once we are done using it
    ssq_free(querier);

#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}

void ssq_info_print(const A2S_INFO *const info) {
    printf("----- INFO BEGIN -----\n");

    printf("Protocol.......: %" PRIu8 "\n", info->protocol);
    printf("Name...........: %s\n", info->name);
    printf("Map............: %s\n", info->map);
    printf("Folder.........: %s\n", info->folder);
    printf("Game...........: %s\n", info->game);
    printf("ID.............: %" PRIu16 "\n", info->id);
    printf("Players........: %" PRIu8 "/%" PRIu8 " (%" PRIu8 " bots)\n", info->players, info->max_players, info->bots);
    printf("Server type....: %s\n",
        ((info->server_type == A2S_SERVER_TYPE_DEDICATED) ? "dedicated" :
        ((info->server_type == A2S_SERVER_TYPE_STV_RELAY) ? "SourceTV relay (proxy)" :
                                                            "non dedicated")));
    printf("Environment....: %s\n",
        ((info->environment == A2S_ENVIRONMENT_WINDOWS) ? "windows" :
        ((info->environment == A2S_ENVIRONMENT_MAC)     ? "mac" :
                                                          "linux")));
    printf("Visibility.....: %s\n", ((info->visibility) ? "private" : "public"));
    printf("VAC............: %s\n", ((info->vac)        ? "secured" : "unsecured"));
    printf("Version........: %s\n", info->version);

    if (info->edf & A2S_INFO_FLAG_PORT)
        printf("Port...........: %" PRIu16 "\n", info->port);

    if (info->edf & A2S_INFO_FLAG_STEAMID)
        printf("SteamID........: %" PRIu64 "\n", info->steamid);

    if (info->edf & A2S_INFO_FLAG_STV) {
        printf("Port (SourceTV): %" PRIu16 "\n", info->stv_port);
        printf("Name (SourceTV): %s\n", info->stv_name);
    }

    if (info->edf & A2S_INFO_FLAG_KEYWORDS)
        printf("Keywords.......: %s\n", info->keywords);

    if (info->edf & A2S_INFO_FLAG_GAMEID)
        printf("GameID.........: %" PRIu64 "\n", info->gameid);

    printf("------ INFO END ------\n");
}

void ssq_player_print(const A2S_PLAYER players[], const uint8_t player_count) {
    for (uint8_t i = 0; i < player_count; ++i) {
        printf("--- PLAYER %" PRIu8 " BEGIN ---\n", i);
        printf("Name....: %s\n", players[i].name);
        printf("Score...: %" PRId32 "\n", players[i].score);
        printf("Duration: %f\n", players[i].duration);
        printf("---- PLAYER %" PRIu8 " END ----\n", i);
    }
}

void ssq_rules_print(const A2S_RULES rules[], const uint16_t rule_count) {
    printf("----- RULES BEGIN -----\n");

    for (uint16_t i = 0; i < rule_count; ++i)
        printf("%s = %s\n", rules[i].name, rules[i].value);

    printf("------ RULES END ------\n");
}
