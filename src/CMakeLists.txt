add_subdirectory(a2s)

target_sources(ssq PRIVATE
    error.c
    packet.c
    query.c
    response.c
    server.c
    stream.c
)
