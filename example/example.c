/*
 * example.c
 *
 * This example program takes a Source game server hostname and optional port number as command line arguments.
 * It sends the 3 following A2S queries to the target synchronously: A2S_INFO, A2S_PLAYER and A2S_RULES.
 * The query responses are pretty-printed to the standard output.
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

/* Pretty-prints an `A2S_INFO' struct. */
void ssq_info_print(const A2S_INFO *info);

/* Pretty-prints an `A2S_PLAYER' array. */
void ssq_player_print(const A2S_PLAYER *players, uint8_t player_count);

/* Pretty-prints an `A2S_RULES' array. */
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
#endif

    const char *hostname = argv[1];
    uint16_t    port;

    if (argc == 3)
        port = atoi(argv[2]);
    else
        port = DEFAULT_PORT;

    /* Initialization */

    // Initializes a new Source server querier.
    SSQ_QUERIER *querier = ssq_init();

    // Asserts that the memory allocation was successful.
    assert(querier != NULL);

    /* Sets the timeout for both receiving and sending operations to 3000 ms.
     * If this function is *not* called, the timeouts for receiving/sending operations will be set to the
     * default values of `SSQ_TIMEOUT_RECV_DEFAULT_VALUE' and `SSQ_TIMEOUT_SEND_DEFAULT_VALUE' respectively. */
    ssq_set_timeout(querier, SSQ_TIMEOUT_RECV | SSQ_TIMEOUT_SEND, 3000);

    // Sets the Source server querier's target.
    ssq_set_target(querier, hostname, port);

    /* `ssq_set_target' error handling */
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_set_target: %s: %s\n", hostname, error_message);

        // IMPORTANT: Always clear the error once we are done handling it.
        ssq_errclr(querier);

        ssq_free(querier);

        exit(EXIT_FAILURE);
    }


    /* A2S_INFO */

    // Sends an A2S_INFO query.
    A2S_INFO *info = ssq_info(querier);

    /* `ssq_info' error handling */
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_info: %s\n", error_message);

        // IMPORTANT: Always clear the error once we are done handling it.
        ssq_errclr(querier);

        ssq_free(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-prints the A2S_INFO query response.
    ssq_info_print(info);

    // IMPORTANT: Always free the `A2S_INFO' struct once we are done using it.
    ssq_info_free(info);


    /* A2S_PLAYER */

    // Sends an A2S_PLAYER query.
    uint8_t player_count = 0;
    A2S_PLAYER *players = ssq_player(querier, &player_count);

    /* `ssq_player' error handling */
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_player: %s\n", error_message);

        // IMPORTANT: Always clear the error once we are done handling it.
        ssq_errclr(querier);

        ssq_free(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-prints the A2S_PLAYER query response.
    ssq_player_print(players, player_count);

    // IMPORTANT: Always free the `A2S_PLAYER' array once we are done using it.
    ssq_player_free(players, player_count);


    /* A2S_RULES */

    // Sends an A2S_RULES query.
    uint16_t rule_count = 0;
    A2S_RULES *rules = ssq_rules(querier, &rule_count);

    /* `ssq_rules' error handling */
    if (!ssq_ok(querier)) {
        const char *error_message = ssq_errm(querier);
        fprintf(stderr, "ssq_rules: %s\n", error_message);

        // IMPORTANT: Always clear the error once we are done handling it.
        ssq_errclr(querier);

        ssq_free(querier);

        exit(EXIT_FAILURE);
    }

    // Pretty-prints the A2S_RULES query response.
    ssq_rules_print(rules, rule_count);

    // IMPORTANT: Always free the `A2S_RULES' array once we are done using it.
    ssq_rules_free(rules, rule_count);


    // IMPORTANT: Always free the Source server querier once we are done using it.
    ssq_free(querier);

#ifdef _WIN32
    WSACleanup();
#endif

    exit(EXIT_SUCCESS);
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

    const char *server_type;
    switch (info->server_type) {
        case A2S_SERVER_TYPE_DEDICATED:     server_type = "dedicated";              break;
        case A2S_SERVER_TYPE_NON_DEDICATED: server_type = "non-dedicated";          break;
        case A2S_SERVER_TYPE_STV_RELAY:     server_type = "SourceTV relay (proxy)"; break;
        default:                            server_type = "?";                      break;
    }
    printf("Server type....: %s\n", server_type);

    const char *environment;
    switch (info->environment) {
        case A2S_ENVIRONMENT_LINUX:   environment = "linux";   break;
        case A2S_ENVIRONMENT_WINDOWS: environment = "windows"; break;
        case A2S_ENVIRONMENT_MAC:     environment = "mac";     break;
        default:                      environment = "?";       break;
    }
    printf("Environment....: %s\n", environment);
    printf("Visibility.....: %s\n", (info->visibility ? "private" : "public"));
    printf("VAC............: %s\n", (info->vac        ? "secured" : "unsecured"));
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
