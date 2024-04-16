add_subdirectory(a2s)
add_subdirectory(server)

target_sources(ssq PUBLIC FILE_SET HEADERS FILES
    a2s.h
    error.h
    helper.h
    packet.h
    query.h
    response.h
    server.h
    stream.h
)
