cmake_minimum_required (VERSION 2.8.11)
project (gateway-core CXX)

find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_NET PocoNet)
find_library (POCO_JSON PocoJSON)
find_library (POCO_XML PocoXML)

add_definitions(-std=c++11)

include_directories(
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/base/src
)

add_executable(beeeon-gateway main.cpp)

set(LIBS
	${POCO_FOUNDATION}
	${POCO_SSL}
	${POCO_CRYPTO}
	${POCO_UTIL}
	${POCO_NET}
	${POCO_JSON}
	${POCO_XML}
)

target_link_libraries(beeeon-gateway
		-Wl,--whole-archive
		BeeeOnBase
		-Wl,--no-whole-archive
		${LIBS}
)

install(TARGETS beeeon-gateway
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)