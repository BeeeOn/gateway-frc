cmake_minimum_required (VERSION 2.8.11)
project (gateway-core CXX)

find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_NET PocoNet)
find_library (POCO_JSON PocoJSON)
find_library (POCO_XML PocoXML)
find_library (PTHREAD pthread)
find_library (ZMQ zmq)
find_library (OPENZWAVE openzwave)

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/commands/DeviceSetValueCommand.cpp
	${PROJECT_SOURCE_DIR}/commands/DeviceSetValueResult.cpp
	${PROJECT_SOURCE_DIR}/commands/DeviceUnpairCommand.cpp
	${PROJECT_SOURCE_DIR}/commands/GatewayListenCommand.cpp
	${PROJECT_SOURCE_DIR}/commands/ServerDeviceListCommand.cpp
	${PROJECT_SOURCE_DIR}/commands/ServerDeviceListResult.cpp
	${PROJECT_SOURCE_DIR}/commands/ServerLastValueCommand.cpp
	${PROJECT_SOURCE_DIR}/commands/ServerLastValueResult.cpp
	${PROJECT_SOURCE_DIR}/core/AbstractCollector.cpp
	${PROJECT_SOURCE_DIR}/core/AbstractDistributor.cpp
	${PROJECT_SOURCE_DIR}/core/Answer.cpp
	${PROJECT_SOURCE_DIR}/core/AnswerQueue.cpp
	${PROJECT_SOURCE_DIR}/core/BasicDistributor.cpp
	${PROJECT_SOURCE_DIR}/core/CollectorDataValues.cpp
	${PROJECT_SOURCE_DIR}/core/Command.cpp
	${PROJECT_SOURCE_DIR}/core/CommandDispatcher.cpp
	${PROJECT_SOURCE_DIR}/core/CommandHandler.cpp
	${PROJECT_SOURCE_DIR}/core/CommandProgressHandler.cpp
	${PROJECT_SOURCE_DIR}/core/CommandRunner.cpp
	${PROJECT_SOURCE_DIR}/core/DeviceDataCollection.cpp
	${PROJECT_SOURCE_DIR}/core/DeviceManager.cpp
	${PROJECT_SOURCE_DIR}/core/DistributorListener.cpp
	${PROJECT_SOURCE_DIR}/core/Exporter.cpp
	${PROJECT_SOURCE_DIR}/core/Result.cpp
	${PROJECT_SOURCE_DIR}/core/SimpleCollector.cpp
	${PROJECT_SOURCE_DIR}/core/VirtualSensor.cpp
	${PROJECT_SOURCE_DIR}/credentials/Credentials.cpp
	${PROJECT_SOURCE_DIR}/credentials/CredentialsStorage.cpp
	${PROJECT_SOURCE_DIR}/credentials/PasswordCredentials.cpp
	${PROJECT_SOURCE_DIR}/credentials/PinCredentials.cpp
	${PROJECT_SOURCE_DIR}/exporters/NamedPipeExporter.cpp
	${PROJECT_SOURCE_DIR}/jablotron/JablotronDeviceManager.cpp
	${PROJECT_SOURCE_DIR}/jablotron/SerialControl.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceManagerID.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleID.cpp
	${PROJECT_SOURCE_DIR}/model/SensorValue.cpp
	${PROJECT_SOURCE_DIR}/net/SOAPMessage.cpp
	${PROJECT_SOURCE_DIR}/net/UPnP.cpp
	${PROJECT_SOURCE_DIR}/util/AsyncExecutor.cpp
	${PROJECT_SOURCE_DIR}/util/CSVSensorDataFormatter.cpp
	${PROJECT_SOURCE_DIR}/util/CSVStatisticsFormatter.cpp
	${PROJECT_SOURCE_DIR}/util/NullSensorDataFormatter.cpp
	${PROJECT_SOURCE_DIR}/util/SensorDataFormatter.cpp
	${PROJECT_SOURCE_DIR}/util/StatisticsData.cpp
	${PROJECT_SOURCE_DIR}/util/StatisticsPrinter.cpp
	${PROJECT_SOURCE_DIR}/util/ZMQUtil.cpp
	${PROJECT_SOURCE_DIR}/z-wave/GenericZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/NotificationProcessor.cpp
	${PROJECT_SOURCE_DIR}/z-wave/ZWaveDeviceManager.cpp
	${PROJECT_SOURCE_DIR}/z-wave/ZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/z-wave/manufacturers/AeotecZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/manufacturers/DLinkZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/manufacturers/FibaroZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/manufacturers/PhilioZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/manufacturers/PoppZWaveMessageFactory.cpp
	${PROJECT_SOURCE_DIR}/z-wave/products/AeotecZW100ZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/z-wave/products/DLinkDchZ120ZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/z-wave/products/FibaroFGK107ZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/z-wave/products/PhilioPST021CZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/z-wave/products/Popp123601ZWaveMessage.cpp
	${PROJECT_SOURCE_DIR}/zmq/FakeHandler.cpp
	${PROJECT_SOURCE_DIR}/zmq/FakeHandlerTest.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQBroker.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQClient.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQConnector.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQDeviceManagerTable.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQMessage.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQMessageError.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQMessageType.cpp
	${PROJECT_SOURCE_DIR}/zmq/ZMQMessageValueType.cpp
)

file(GLOB TESTING_SOURCES
	${PROJECT_SOURCE_DIR}/core/TestingCenter.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/base/src
)

add_library(BeeeOnGateway ${SOURCES})
add_library(BeeeOnTesting ${TESTING_SOURCES})

add_executable(beeeon-gateway main.cpp)

set(LIBS
	${POCO_FOUNDATION}
	${POCO_SSL}
	${POCO_CRYPTO}
	${POCO_UTIL}
	${POCO_NET}
	${POCO_JSON}
	${POCO_XML}
	${PTHREAD}
	${ZMQ}
	${OPENZWAVE}
)

target_link_libraries(beeeon-gateway
		-Wl,--whole-archive
		BeeeOnGateway
		BeeeOnTesting
		BeeeOnBase
		-Wl,--no-whole-archive
		${LIBS}
)

install(TARGETS beeeon-gateway BeeeOnGateway BeeeOnTesting
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
