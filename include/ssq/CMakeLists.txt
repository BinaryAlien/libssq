add_subdirectory(a2s)

target_sources(ssq PUBLIC FILE_SET HEADERS FILES
  error.h
  server.h
  timeout.h
)
