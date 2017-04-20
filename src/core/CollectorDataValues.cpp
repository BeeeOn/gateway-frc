#include <vector>

#include "core/CollectorDataValues.h"
#include "model/ModuleID.h"
#include "model/SensorValue.h"
#include "util/IncompleteTimestamp.h"

using namespace BeeeOn;

CollectorTimeValue::CollectorTimeValue(const IncompleteTimestamp &time,
		const double &value,
		const bool &valid):
	m_time(time),
	m_value(value),
	m_valid(valid)
{
}

IncompleteTimestamp CollectorTimeValue::time() const
{
	return m_time;
}
double CollectorTimeValue::value() const
{
	return m_value;
}

bool CollectorTimeValue::isValid() const
{
	return m_valid;
}

CollectorDataValues::CollectorDataValues(const ModuleID moduleID):
	m_moduleID(moduleID)
{
}

ModuleID CollectorDataValues::moduleID() const
{
	return m_moduleID;
}

void CollectorDataValues::insert(const IncompleteTimestamp &time, const double &value, const bool &valid)
{
	if (valid)
		m_valueInTime.push_back(CollectorTimeValue(time, value));
	else
		m_valueInTime.push_back(CollectorTimeValue(time, value, false));
}

std::vector<CollectorTimeValue>::const_iterator CollectorDataValues::begin() const
{
	return m_valueInTime.begin();
}

std::vector<CollectorTimeValue>::const_iterator CollectorDataValues::end() const
{
	return m_valueInTime.end();
}

bool CollectorDataValues::operator==(const SensorValue &value) const
{
	return value.moduleID() == moduleID();
}

bool CollectorDataValues::operator!=(const SensorValue &value) const
{
	return value.moduleID() != moduleID();
}
