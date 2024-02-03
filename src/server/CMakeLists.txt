
aux_source_directory(. SRC_LIST)
aux_source_directory(./mysql MYSQL_LIST)
aux_source_directory(./redis REDIS_LIST)
aux_source_directory(./model MODEL_LIST)


#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST} ${MYSQL_LIST}  ${REDIS_LIST} ${MODEL_LIST})

# 指定可执行文件需要依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)