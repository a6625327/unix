# 可执行程序目录
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

# 所需要包含的动态库，分别有线程、日志、配置解析
set(LINK_LIBRATIES zlog pthread iniparser) 
# 可执行程序一共有三个，serv、recv、send
set(EXEC_BIN serv recv send) 

# 可执行的主程序需要链接的目标文件目录
aux_source_directory(${PROJECT_SOURCE_DIR}/src/util/ serv_src)

# 头文件寻找目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/iniparser
)

# message("the serv_src: " ${serv_src})

# 动态库寻找目录
link_directories(
    /usr/local/util
    ${PROJECT_SOURCE_DIR}/util
)

# 可执行的主程序需要链接的目标文件目录
add_library(obj OBJECT ${serv_src})

# 链接包含main函数的主程序文件，生成相应的可执行文件
add_executable(serv server.c $<TARGET_OBJECTS:obj>)
add_executable(recv recv.c   $<TARGET_OBJECTS:obj>)
add_executable(send send.c   $<TARGET_OBJECTS:obj>)
# add_executable(ttt ttt.c     $<TARGET_OBJECTS:obj>)
# add_executable(ddd ddd.c)

# 将所有可执行程序链接动态库
foreach(item ${EXEC_BIN})
    target_link_libraries(${item}
        ${LINK_LIBRATIES}
    )
endforeach(item ${EXEC_BIN})

# set_target_properties(serv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(recv PROPERTIES LINKER_LANGUAGE c)
# set_target_properties(send PROPERTIES LINKER_LANGUAGE c)