set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}")

aux_source_directory(${PROJECT_SOURCE_DIR}/src/lib/ serv_src)

include_directories(${PROJECT_SOURCE_DIR}/include)

message("the serv_src: " ${serv_src})

LINK_DIRECTORIES(
    /usr/local/lib
    ${PROJECT_SOURCE_DIR}/lib
)

ADD_EXECUTABLE(serv server.c ${serv_src})
ADD_EXECUTABLE(recv recv.c ${serv_src})
ADD_EXECUTABLE(send send.c ${serv_src})

target_link_libraries(serv
    zlog
    pthread
)

target_link_libraries(recv
    zlog
    pthread
)

target_link_libraries(send
    zlog
    pthread
)
# set_target_properties(serv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(recv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(send PROPERTIES LINKER_LANGUAGE c)