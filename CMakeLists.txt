cmake_minimum_required(VERSION 3.30)
project(chat-app C)

set(CMAKE_C_STANDARD 11)

add_executable(server SERVER/src/server.c)

add_executable(client CLIENT/src/client.c)
