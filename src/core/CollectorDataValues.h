#pragma once

#include <vector>

#include "model/ModuleID.h"
#include "model/SensorValue.h"
#include "util/IncompleteTimestamp.h"

namespace BeeeOn {

/**
 * @brief The CollectorTimeValue class
 * This class only unite timestamp with value (and validity) together
 */
class CollectorTimeValue {
public:
	CollectorTimeValue(const IncompleteTimestamp &time,
			const double &value,
			const bool &valid = true);

	IncompleteTimestamp time() const;

	double value() const;

	bool isValid() const;

private:
	IncompleteTimestamp m_time;
	double m_value;
	bool m_valid;
};

/**
 * @brief The CollectorDataValues class
 * Store data (time, value, valid) for one module
 */
class CollectorDataValues {
public:
	CollectorDataValues(const ModuleID moduleID);

	ModuleID moduleID() const;

	void insert(const IncompleteTimestamp &time,
			const double &value, const bool &valid);

	std::vector<CollectorTimeValue>::const_iterator begin() const;

	std::vector<CollectorTimeValue>::const_iterator end() const;

	bool operator==(const SensorValue &value) const;

	bool operator!=(const SensorValue &value) const;

private:
	ModuleID m_moduleID;
	std::vector<CollectorTimeValue> m_valueInTime;
};

}
