#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include <Poco/Timestamp.h>

#include "core/DeviceDataCollection.h"
#include "di/Injectable.h"
#include "util/StatisticsData.h"
#include "util/StatisticsPrinter.h"

BEEEON_OBJECT_BEGIN(BeeeOn, StatisticsPrinter)
BEEEON_OBJECT_CASTABLE(StatisticsPrinter)
BEEEON_OBJECT_REF("formatter", &StatisticsPrinter::setFormatter)
BEEEON_OBJECT_TEXT("outputFile", &StatisticsPrinter::setOutputFile)
BEEEON_OBJECT_END(BeeeOn, StatisticsPrinter)

using namespace BeeeOn;
using namespace Poco;
using namespace std;

StatisticsPrinter::StatisticsPrinter()
{
}

StatisticsPrinter::~StatisticsPrinter()
{
}

void StatisticsPrinter::printCollectionBegin()
{
	stream() << m_formatter->collectionBegin();
	stream().flush();
}

void StatisticsPrinter::printCollectionEnd(
		const int devicesCount,
		const int modulesCount
		)
{
	stream() << m_formatter->collectionEnd(devicesCount, modulesCount);
	stream().flush();
}

void StatisticsPrinter::printEntry(const StatisticsData &stat)
{
	stream() << m_formatter->formatModuleStat(stat);
	stream().flush();
}

void StatisticsPrinter::setOutputFile(const string &filename)
{
	m_filename = filename;
}

void StatisticsPrinter::setFormatter(Poco::SharedPtr<StatisticsFormatter> formatter)
{
	m_formatter = formatter;
}

ostream& StatisticsPrinter::stream()
{
	if (!m_stream.isNull())
		return *m_stream;

	if (m_filename.empty())
		return cout;

	m_stream = new ofstream(m_filename, ios::app);
	return *m_stream;
}
