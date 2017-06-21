#include <string>

#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include "core/AbstractCollector.h"

using namespace BeeeOn;

AbstractCollector::AbstractCollector():
	m_exportInterval(Poco::Timespan(DEFAULT_EXPORT_SECONDS, 0))
{
}

AbstractCollector::~AbstractCollector()
{
}

void AbstractCollector::run()
{
	while (!m_stopEvent.tryWait(m_exportInterval.totalMilliseconds())) {
		exportStatistics(m_exportInterval);
	}
}

void AbstractCollector::stop()
{
	m_stopEvent.set();
}

void AbstractCollector::setExportInterval(int seconds)
{
	if (seconds < 1)
		throw Poco::InvalidArgumentException("invalid export interval, must be greater than zero");

	m_exportInterval = Poco::Timespan(seconds, 0);
}

void AbstractCollector::exportStatistics(const Poco::Timespan span)
{
	poco_debug(logger(),"exporting statistics of last "
		+ std::to_string(span.totalSeconds())
		+ " seconds");

	m_printer->printCollectionBegin();
	std::vector<StatisticsData> data = computeLastStats(span);

	for (auto const &stat : data)
		m_printer->printEntry(stat);

	m_printer->printCollectionEnd(
		devicesCount(),
		modulesCount()
		);
}

void AbstractCollector::setPrinter(Poco::SharedPtr<StatisticsPrinter> printer)
{
	m_printer = printer;
}
