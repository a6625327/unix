set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LINK_LIBRATIES zlog pthread)
set(EXEC_BIN serv recv send)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/lib/ serv_src)

include_directories(${PROJECT_SOURCE_DIR}/include)

message("the serv_src: " ${serv_src})

link_directories(
    /usr/local/lib
    ${PROJECT_SOURCE_DIR}/lib
)

add_library(obj OBJECT ${serv_src})


add_executable(serv server.c $<TARGET_OBJECTS:obj>)
add_executable(recv recv.c   $<TARGET_OBJECTS:obj>)
add_executable(send send.c   $<TARGET_OBJECTS:obj>)

foreach(item ${EXEC_BIN})
    target_link_libraries(${item}
        ${LINK_LIBRATIES}
    )
endforeach(item ${EXEC_BIN})

# set_target_properties(serv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(recv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(send PROPERTIES LINKER_LANGUAGE c)