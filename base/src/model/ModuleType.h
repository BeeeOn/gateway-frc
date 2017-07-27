#ifndef BEEEON_MODULE_TYPE_H
#define BEEEON_MODULE_TYPE_H

#include <set>

#include "util/Enum.h"

namespace BeeeOn {

/**
 * Representation of value type that device can send. Each value
 * consists of two parts: Type and Attribute.
 *
 * The attribute is optional. The type is high level data type
 * and the attribute is additional information, for example
 * location (inner, outer).
 */
class ModuleType {
public:
	struct AttributeEnum {
		enum Raw {
			/**
			 * The sensor/actuator is located inside (a building).
			 */
			TYPE_INNER,
			/**
			 * The sensor enables to set (to modify) value only from sensor.
			 */
			TYPE_MANUAL_REQUESTED,
			/**
			 * The sensor/actuator is located outside.
			 */
			TYPE_OUTER,
		};

		static EnumHelper<Raw>::ValueMap &valueMap();
	};

	struct TypeEnum {
		enum Raw {
			/**
			 * Range: 0..100
			 * Unit: %
			 */
			TYPE_BATTERY,
			/**
			 * Min: 0
			 * Unit: one part per million (ppm)
			 */
			TYPE_CO2,
			/**
			 * Value 0: no fire detected
			 * Value 1: fire detected
			 */
			TYPE_FIRE,
			/**
			 * Range: 0..100
			 * Unit: %
			 */
			TYPE_HUMIDITY,
			/**
			 * Value 0: no motion detected
			 * Value 1: motion detected
			 */
			TYPE_MOTION,
			/**
			 * Unit: dB
			 */
			TYPE_NOISE,
			/**
			 * Value 0: open
			 * Value 1: closed
			 */
			TYPE_OPEN_CLOSE,
			/**
			 * Value 0: off
			 * Value 1: on
			 */
			TYPE_ON_OFF,
			/**
			 * Unit: hPa
			 */
			TYPE_PRESSURE,
			/**
			 * Value 0: no security alert
			 * Value 1: security alert is active
			 *
			 */
			TYPE_SECURITY_ALERT,
			/**
			 * Value 0: no shake detect
			 * Value 1: shake detect
			 */
			TYPE_SHAKE,
			/**
			 * Unit: °C
			 */
			TYPE_TEMPERATURE,
		};

		static EnumHelper<Raw>::ValueMap &valueMap();
	};

	typedef Enum<TypeEnum> Type;
	typedef Enum<AttributeEnum> Attribute;

	ModuleType(const Type &type);
	ModuleType(const Type &type, const std::set<Attribute> &attributes);

	void setType(const Type &type);
	Type type() const;

	void setAttributes(const std::set<Attribute> &attributes);
	std::set<Attribute> attributes() const;

	static ModuleType parse(std::string input);

private:
	Type m_type;
	std::set<Attribute> m_attributes;
};

}

#endif
