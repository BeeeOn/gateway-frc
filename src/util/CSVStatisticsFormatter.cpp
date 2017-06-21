#include <vector>

#include <Poco/NumberFormatter.h>
#include <Poco/Timestamp.h>

#include "core/DeviceDataCollection.h"
#include "di/Injectable.h"
#include "util/CSVStatisticsFormatter.h"
#include "util/StatisticsData.h"

#define DEFAULT_SEPARATOR ";"
#define PRECISION_OF_VALUE 2

BEEEON_OBJECT_BEGIN(BeeeOn, CSVStatisticsFormatter)
BEEEON_OBJECT_CASTABLE(StatisticsFormatter)
BEEEON_OBJECT_TEXT("separator", &CSVStatisticsFormatter::setSeparator);
BEEEON_OBJECT_END(BeeeOn, CSVStatisticsFormatter)

using namespace BeeeOn;
using namespace Poco;
using namespace std;

CSVStatisticsFormatter::CSVStatisticsFormatter():
	m_separator(DEFAULT_SEPARATOR)
{
}

string CSVStatisticsFormatter::collectionBegin()
{
	string result = "";
	result = "DeviceID" + m_separator
		+ "ModuleID" + m_separator
		+ "Min" + m_separator
		+ "Max" + m_separator
		+ "Avg" + m_separator
		+ "Median" + m_separator
		+ "Count" + m_separator
		+ "NonValid" + m_separator
		+ "\n";

	return result;
}

string CSVStatisticsFormatter::collectionEnd(
		const int devicesCount,
		const int modulesCount
		)
{
	string result = "";
	result = result
		+ "Devices_sum" + m_separator
		+ "Modules_sum" + m_separator
		+ "\n";

	result = result
		+ to_string(devicesCount) + m_separator
		+ to_string(modulesCount) + m_separator
		+ "\n";

	return result;
}

string CSVStatisticsFormatter::formatModuleStat(
		const StatisticsData &stat
		)
{
	string result = "";
	if (!stat.isEmpty()) {
		result = result + stat.deviceID().toString() + m_separator
			+ stat.moduleID().toString() + m_separator
			+ NumberFormatter::format(stat.minimum(), PRECISION_OF_VALUE) + m_separator
			+ NumberFormatter::format(stat.maximum(), PRECISION_OF_VALUE) + m_separator
			+ NumberFormatter::format(stat.average(), PRECISION_OF_VALUE) + m_separator
			+ NumberFormatter::format(stat.median(), PRECISION_OF_VALUE) + m_separator
			+ NumberFormatter::format(stat.size(), PRECISION_OF_VALUE) + m_separator
			+ NumberFormatter::format(stat.invalidValues(), PRECISION_OF_VALUE) + m_separator
			+ "\n";
	}

	return result;
}

void CSVStatisticsFormatter::setSeparator(const string &separator)
{
	m_separator = separator;
}
