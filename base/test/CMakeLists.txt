cmake_minimum_required (VERSION 2.8.11)
project (base-test CXX)

find_library (CPP_UNIT cppunit)
find_library (PTHREAD pthread)
find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_NET PocoNet)
find_library (POCO_XML PocoXML)
find_library (POCO_JSON PocoJSON)

add_definitions(-std=c++11)
add_definitions(-Wall -pedantic -Wextra)

file(GLOB TEST_LIBRARY_SOURCES
	${PROJECT_SOURCE_DIR}/cppunit/TapTestProducer.cpp
)

file(GLOB TEST_SOURCES
	${PROJECT_SOURCE_DIR}/di/DependencyInjectorTest.cpp
	${PROJECT_SOURCE_DIR}/di/DIWrapperTest.cpp
	${PROJECT_SOURCE_DIR}/di/DIDaemonTest.cpp
	${PROJECT_SOURCE_DIR}/io/ConsoleTest.cpp
	${PROJECT_SOURCE_DIR}/io/TCPConsoleTest.cpp
	${PROJECT_SOURCE_DIR}/loop/LoopRunnerTest.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayIDTest.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceIDTest.cpp
	${PROJECT_SOURCE_DIR}/util/BacktraceTest.cpp
	${PROJECT_SOURCE_DIR}/util/ClassInfoTest.cpp
	${PROJECT_SOURCE_DIR}/util/DAMMTest.cpp
	${PROJECT_SOURCE_DIR}/util/EnumTest.cpp
	${PROJECT_SOURCE_DIR}/util/Base64Test.cpp
	${PROJECT_SOURCE_DIR}/util/SecureXmlParserTest.cpp
	${PROJECT_SOURCE_DIR}/util/TypesSAXHandlerTest.cpp
	${PROJECT_SOURCE_DIR}/util/DevicesSAXHandlerTest.cpp
	${PROJECT_SOURCE_DIR}/util/WithTraceTest.cpp
	${PROJECT_SOURCE_DIR}/ssl/X509FingerprintTest.cpp
	${PROJECT_SOURCE_DIR}/util/JsonUtilTest.cpp
)

file(GLOB TEST_WORK_SOURCES
	${PROJECT_SOURCE_DIR}/work/BasicProcessorTest.cpp
	${PROJECT_SOURCE_DIR}/work/BasicQueueTest.cpp
	${PROJECT_SOURCE_DIR}/work/GenericWorkRunnerTest.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../src
)

add_library(BeeeOnBaseTest SHARED ${TEST_LIBRARY_SOURCES})
add_executable(test-suite-base test.cpp ${TEST_SOURCES} ${TEST_WORK_SOURCES})
add_test(COMMAND test-suite-base)

set(LIBS
	${POCO_FOUNDATION}
	${POCO_SSL}
	${POCO_CRYPTO}
	${POCO_UTIL}
	${POCO_NET}
	${POCO_XML}
	${POCO_JSON}
	${CPP_UNIT}
	${PTHREAD}
)

# Apple's linker doesn't support --whole-archive. Instead it uses -all_load.
if (APPLE)
	target_link_libraries(test-suite-base
		-Wl,-all_load
		BeeeOnBaseTest
		BeeeOnWork
		BeeeOnBase
		${LIBS}
	)
# The -Wl,--whole-archive is necessary for all libraries defining
# BEEEON_OBJECTs (see di/Injectable.h).
else()
	target_link_libraries(test-suite-base
		-Wl,--whole-archive
		BeeeOnBaseTest
		BeeeOnWork
		BeeeOnBase
		-Wl,--no-whole-archive
		${LIBS}
	)
endif()

install(TARGETS test-suite-base BeeeOnBaseTest
	RUNTIME DESTINATION share/beeeon/test-suite
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	CONFIGURATIONS Debug
)
