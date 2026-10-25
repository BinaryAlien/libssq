add_subdirectory(a2s)

target_sources(ssq PRIVATE
  buffer.c
  cursor.c
  error.c
  packet.c
  query.c
  response.c
  server.c
  socket.c
  timeout.c
)
