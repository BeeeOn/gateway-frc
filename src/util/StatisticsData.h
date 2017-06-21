#pragma once

#include <vector>

#include "model/DeviceID.h"
#include "model/ModuleID.h"

namespace BeeeOn {

/**
 * This class is filled with records of sensor's values and provides
 * statistical information of collected values.
 */
class StatisticsData
{
public:
	StatisticsData(DeviceID deviceID, ModuleID moduleID);

	/**
	 * @brief insertInvalid
	 * Notify about invalid value
	 */
	void insertInvalid();

	/**
	 * @brief insertSample
	 * @param value
	 * Insert record of value for analysis
	 */
	void insertSample(const double &value);

	double median() const;

	bool isEmpty() const;

	double average() const;

	int size() const;

	ModuleID moduleID() const;

	DeviceID deviceID() const;

	double minimum() const;

	double maximum() const;

	unsigned long invalidValues() const;

	std::vector<double> samples() const;

private:
	double m_maxValue;
	double m_minValue;
	double m_sum;
	unsigned long m_invalidValues;
	DeviceID m_deviceID;
	ModuleID m_moduleID;
	std::vector<double> m_values;
};

}
