#include <algorithm>
#include <cmath>
#include <limits>

#include <Poco/Exception.h>

#include "util/StatisticsData.h"

using namespace BeeeOn;
using namespace std;

StatisticsData::StatisticsData(DeviceID deviceID, ModuleID moduleID) :
	m_maxValue(numeric_limits<double>::min()),
	m_minValue(numeric_limits<double>::max()),
	m_sum(0),
	m_invalidValues(0),
	m_deviceID(deviceID),
	m_moduleID(moduleID)
{
}

void StatisticsData::insertInvalid()
{
	m_invalidValues++;
}

void StatisticsData::insertSample(const double &value)
{
	m_values.push_back(value);

	if (value > m_maxValue)
		m_maxValue = value;
	if (value < m_minValue)
		m_minValue = value;
	m_sum += value;
}

double StatisticsData::median() const
{
	std::vector<double> tmp = m_values;

	sort(tmp.begin(), tmp.end());
	if (size() % 2 == 0)
		return (tmp[(size()/2)-1] + tmp[(size()/2)]) / 2;
	else
		return tmp[(size()/2)-1];
}

bool StatisticsData::isEmpty() const
{
	return (size() > 0 ? false : true);
}

double StatisticsData::average() const
{
	if (size() == 0)
		return NAN;

	return m_sum / size();
}

int StatisticsData::size() const
{
	return m_values.size();
}

ModuleID StatisticsData::moduleID() const
{
	return m_moduleID;
}

DeviceID StatisticsData::deviceID() const
{
	return m_deviceID;
}

double StatisticsData::minimum() const
{
	return m_minValue;
}

double StatisticsData::maximum() const
{
	return m_maxValue;
}

std::vector<double> StatisticsData::samples() const
{
	return m_values;
}

unsigned long StatisticsData::invalidValues() const
{
	return m_invalidValues;
}
