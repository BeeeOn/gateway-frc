#pragma once

#include <string>
#include <map>

#include "util/StatisticsFormatter.h"

namespace BeeeOn {

class DeviceDataCollection;

class CSVStatisticsFormatter : public StatisticsFormatter {
public:
	CSVStatisticsFormatter();

	std::string collectionBegin() override;
	std::string collectionEnd(
		const int devicesCount,
		const int modulesCount
		) override;

	std::string formatModuleStat(
		const StatisticsData &stat
		) override ;

	void setSeparator(const std::string &separator);
private:
	std::string m_separator;
};

}
