aux_source_directory(. SRCS) # 这行代码的作用是查找当前目录（.）中的所有源文件（.cpp、.c 等），并将它们存储在变量 SRCS 中。
add_executable(Live ${SRCS}) # 这行代码创建一个名为 Live 的可执行文件，并将 SRCS 中的源文件添加到这个目标中。

#add_dependencies(base jsoncpp)  # 添加依赖
target_link_libraries(Live base network)

# 这是生成的 make install命令
# TARGETS Live 要install的文件
# RUNTIME 可执行文件
# DESTINATION sbin 安装到sbin
install(TARGETS Live RUNTIME DESTINATION sbin)