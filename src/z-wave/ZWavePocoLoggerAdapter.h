#pragma once

#include <Poco/Logger.h>

#include <openzwave/platform/unix/LogImpl.h>

namespace BeeeOn {

class ZWavePocoLoggerAdapter : public OpenZWave::i_LogImpl {
public:
	ZWavePocoLoggerAdapter(Poco::Logger& logger);

	/**
	 * Add a node id to the log dump. Skipping node id USB dongle.
	 * Dongle node id is 0. These logs are used in openzwave library,
	 * where there is no need node id.
	 * @param nodeId identifier in the z-wave network
	 * @return string with node id
	 */
	std::string nodeIdString(uint8 const nodeId);

	/**
	 * Write formatted data from variable argument list to string.
	 * @param logLevel log level used in openzwave
	 * @param nodeId idetifier in the z-wave network
	 * @param format message for logger
	 * @param args
	 */
	void Write(OpenZWave::LogLevel logLevel, uint8 const nodeId,
		char const* format, va_list args) override;

	/**
	 * Method convert from OpenZWave log level to Poco log level and
	 * write message to the Poco logger.
	 * @param level log level
	 * @param logMessage message which is written
	 */
	void writeLogImpl(OpenZWave::LogLevel level, std::string logMessage);

	void QueueDump() override;
	void QueueClear() override;

	void SetLoggingState(OpenZWave::LogLevel saveLevel,
		OpenZWave::LogLevel queueLevel,
		OpenZWave::LogLevel dumpTrigger) override;

	void SetLogFileName(const std::string& filename) override;

private:
	Poco::Logger& logger;
};

}
