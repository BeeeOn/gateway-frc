#pragma once

#include <map>
#include <string>

#include "util/StatisticsData.h"

namespace BeeeOn {

class DeviceDataCollection;
class DeviceID;

/*
 * Base interface supporting converting
 * collected statistics data to string.
 * */
class StatisticsFormatter {
public:
	virtual std::string collectionBegin() = 0;
	virtual std::string collectionEnd(
		const int devicesCount,
		const int modulesCount
		) = 0;

	virtual std::string formatModuleStat(
		const StatisticsData &stat
		) = 0;
};

}
