#pragma once

#include <iosfwd>
#include <map>
#include <string>

#include <Poco/SharedPtr.h>

#include "util/StatisticsFormatter.h"

namespace BeeeOn {

class Collector;
class DeviceDataCollection;
class DeviceID;

/**
 * Printer for Collector
 * Through the stored data and prints to stdout or to a file
 * Now in csv format
 */
class StatisticsPrinter
{
public:
	StatisticsPrinter();
	~StatisticsPrinter();

	void printCollectionBegin();
	void printCollectionEnd(const int devicesCount, const int modulesCount);
	void printEntry(const StatisticsData &stat);
	/**
	 * @brief setOutputFile
	 * @param filename - when is empty, than prints to stdout
	 * Determines output, default is stdout
	 */
	void setOutputFile(const std::string &filename);

	void setFormatter(Poco::SharedPtr<StatisticsFormatter> formatter);

private:
	std::string m_filename;
	Poco::SharedPtr<std::ostream> m_stream;

	Poco::SharedPtr<StatisticsFormatter> m_formatter;

	std::ostream &stream();
};

}
