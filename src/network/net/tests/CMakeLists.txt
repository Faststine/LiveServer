add_executable(EventLoopThreadTest EventLoopThreadTest.cpp)
target_link_libraries(EventLoopThreadTest base network)

add_executable(InetAddressTest InetAddressTest.cpp)
target_link_libraries(InetAddressTest base network)

add_executable(SockOptTest SockOptTest.cpp)
target_link_libraries(SockOptTest base network)

add_executable(AcceptorTest AcceptorTest.cpp)
target_link_libraries(AcceptorTest base network)

add_executable(TcpConnectTest TcpConnectTest.cpp)
target_link_libraries(TcpConnectTest base network)