add_subdirectory(a2s)

target_sources(ssq PUBLIC FILE_SET HEADERS FILES
    a2s.h
    error.h
    server.h
)
