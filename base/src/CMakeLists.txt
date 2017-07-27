cmake_minimum_required (VERSION 2.8.11)
project (base-library CXX)

find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_NET PocoNet)
find_library (POCO_XML PocoXML)
find_library (POCO_JSON PocoJSON)

add_definitions(-std=c++11)
add_definitions(-Wall -pedantic -Wextra)

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/di/DependencyInjector.cpp
	${PROJECT_SOURCE_DIR}/di/DIApplicationConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/di/DIDaemon.cpp
	${PROJECT_SOURCE_DIR}/di/DIWrapper.cpp
	${PROJECT_SOURCE_DIR}/di/DIXmlLoader.cpp
	${PROJECT_SOURCE_DIR}/io/Console.cpp
	${PROJECT_SOURCE_DIR}/io/Printable.cpp
	${PROJECT_SOURCE_DIR}/io/SerialPort.cpp
	${PROJECT_SOURCE_DIR}/io/SimpleConsole.cpp
	${PROJECT_SOURCE_DIR}/io/StdConsole.cpp
	${PROJECT_SOURCE_DIR}/io/TCPConsole.cpp
	${PROJECT_SOURCE_DIR}/loop/LoopRunner.cpp
	${PROJECT_SOURCE_DIR}/loop/StoppableLoop.cpp
	${PROJECT_SOURCE_DIR}/loop/StoppableRunnable.cpp
	${PROJECT_SOURCE_DIR}/math/SimpleCalc.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceID.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceInfo.cpp
	${PROJECT_SOURCE_DIR}/model/DevicePrefix.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayID.cpp
	${PROJECT_SOURCE_DIR}/model/GlobalID.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleID.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleInfo.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleType.cpp
	${PROJECT_SOURCE_DIR}/model/SensorValue.cpp
	${PROJECT_SOURCE_DIR}/model/SimpleID.cpp
	${PROJECT_SOURCE_DIR}/model/TypeInfo.cpp
	${PROJECT_SOURCE_DIR}/net/HTTPEntireResponse.cpp
	${PROJECT_SOURCE_DIR}/net/IPAddressRange.cpp
	${PROJECT_SOURCE_DIR}/net/MACAddress.cpp
	${PROJECT_SOURCE_DIR}/provider/DeviceInfoProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/TypeInfoProvider.cpp
	${PROJECT_SOURCE_DIR}/ssl/RejectCertificateHandler.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLClient.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLFacility.cpp
	${PROJECT_SOURCE_DIR}/ssl/SSLServer.cpp
	${PROJECT_SOURCE_DIR}/ssl/X509Fingerprint.cpp
	${PROJECT_SOURCE_DIR}/util/ApplicationConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/util/ArgsParser.cpp
	${PROJECT_SOURCE_DIR}/util/AutoConfigurationExplorer.cpp
	${PROJECT_SOURCE_DIR}/util/Backtrace.cpp
	${PROJECT_SOURCE_DIR}/util/ClassInfo.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationExplorer.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationLoader.cpp
	${PROJECT_SOURCE_DIR}/util/ConfigurationSaver.cpp
	${PROJECT_SOURCE_DIR}/util/CryptoConfig.cpp
	${PROJECT_SOURCE_DIR}/util/CryptoParams.cpp
	${PROJECT_SOURCE_DIR}/util/DAMM.cpp
	${PROJECT_SOURCE_DIR}/util/DevicesSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/FileLoader.cpp
	${PROJECT_SOURCE_DIR}/util/HavingThreadPool.cpp
	${PROJECT_SOURCE_DIR}/util/JsonUtil.cpp
	${PROJECT_SOURCE_DIR}/util/Loggable.cpp
	${PROJECT_SOURCE_DIR}/util/Occasionally.cpp
	${PROJECT_SOURCE_DIR}/util/PosixSignal.cpp
	${PROJECT_SOURCE_DIR}/util/SAXHelper.cpp
	${PROJECT_SOURCE_DIR}/util/SecureXmlParser.cpp
	${PROJECT_SOURCE_DIR}/util/Template.cpp
	${PROJECT_SOURCE_DIR}/util/TimeInterval.cpp
	${PROJECT_SOURCE_DIR}/util/TypesSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/ValueGenerator.cpp
	${PROJECT_SOURCE_DIR}/util/WithTrace.cpp
)

file(GLOB WORK_SOURCES
	${PROJECT_SOURCE_DIR}/work/AbstractWorkBackup.cpp
	${PROJECT_SOURCE_DIR}/work/BasicProcessor.cpp
	${PROJECT_SOURCE_DIR}/work/BasicQueue.cpp
	${PROJECT_SOURCE_DIR}/work/DefaultWorkFacade.cpp
	${PROJECT_SOURCE_DIR}/work/GenericWorkRunner.cpp
	${PROJECT_SOURCE_DIR}/work/Work.cpp
	${PROJECT_SOURCE_DIR}/work/WorkBackup.cpp
	${PROJECT_SOURCE_DIR}/work/WorkContent.cpp
	${PROJECT_SOURCE_DIR}/work/WorkExecutor.cpp
	${PROJECT_SOURCE_DIR}/work/WorkFacade.cpp
	${PROJECT_SOURCE_DIR}/work/WorkFactory.cpp
	${PROJECT_SOURCE_DIR}/work/WorkLockManager.cpp
	${PROJECT_SOURCE_DIR}/work/WorkRunner.cpp
	${PROJECT_SOURCE_DIR}/work/WorkScheduler.cpp
	${PROJECT_SOURCE_DIR}/work/WorkSuspendThrowable.cpp
)

file(GLOB TRANSACTION_SOURCES
	${PROJECT_SOURCE_DIR}/transaction/NoTransactionFactory.cpp
	${PROJECT_SOURCE_DIR}/transaction/NullTransactionManager.cpp
	${PROJECT_SOURCE_DIR}/transaction/ThreadLocalTransactionManager.cpp
	${PROJECT_SOURCE_DIR}/transaction/Transactional.cpp
	${PROJECT_SOURCE_DIR}/transaction/TransactionManager.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}
)

add_library(BeeeOnBase SHARED ${SOURCES})
add_library(BeeeOnWork SHARED ${WORK_SOURCES})
add_library(BeeeOnTransaction SHARED ${TRANSACTION_SOURCES})

install(TARGETS BeeeOnBase BeeeOnWork BeeeOnTransaction
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
