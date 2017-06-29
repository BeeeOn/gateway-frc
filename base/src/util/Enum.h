#ifndef BEEEON_ENUM_H
#define BEEEON_ENUM_H

#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Random.h>

#include <map>
#include <string>

namespace BeeeOn {

/**
 * Provide enum-specific helper data types.
 */
template <typename Raw>
struct EnumHelper {
	/**
	 * ValueMap that each Enum must specify.
	 */
	typedef std::map<Raw, std::string> ValueMap;
	/**
	 * A representation of Enum value. It points to a ValueMap entry
	 * to access both the Raw value and its name.
	 */
	typedef typename ValueMap::const_iterator Value;
	/**
	 * RawMap to lookup Enum instance by Raw value.
	 */
	typedef std::map<Raw, Value> RawMap;
	/**
	 * NamesMap to lookup Enum instance by name.
	 */
	typedef std::map<std::string, Value> NamesMap;

	static RawMap initRawMap(const ValueMap &valueMap)
	{
		RawMap rawMap;

		for (Value it = valueMap.begin(); it != valueMap.end(); ++it) {
			auto result = rawMap.insert(std::make_pair(it->first, it));
			if (!result.second)
				throw Poco::ExistsException(
					"duplicate raw value " + std::to_string(it->first));
		}

		return rawMap;
	}

	static NamesMap initNamesMap(const ValueMap &valueMap)
	{
		NamesMap namesMap;

		for (Value it = valueMap.begin(); it != valueMap.end(); ++it) {
			auto result = namesMap.insert(std::make_pair(it->second, it));
			if (!result.second)
				throw Poco::ExistsException(
					"duplicate string value " + it->second);
		}

		return namesMap;
	}
};

/**
 * Abstract Enum type to simplify defining "clever" enums.
 * A "clever" enum can be deserialized (parsed) from string
 * and serialized back (toString).
 *
 * Base is the user defined class containing an embedded enum
 * called Raw. Example:
 *
 *   class TestEnum {
 *   public:
 *      enum Raw {
 *         TEST_X0,
 *         TEST_X1,
 *         ...
 *      }
 *   };
 *
 *   typedef Enum<TestEnum> Test;
 *
 * Use Test class as the target enum's type.
 */
template <typename Base, typename RawType = typename Base::Raw>
class Enum : public Base {
public:
	typedef RawType Raw;

	typedef typename EnumHelper<Raw>::ValueMap ValueMap;
	typedef typename EnumHelper<Raw>::Value Value;

private:
	Enum(const Value &value):
		m_value(value)
	{
	}

protected:
	static typename EnumHelper<Raw>::RawMap &rawMap()
	{
		static typename EnumHelper<Raw>::RawMap rawMap(
			EnumHelper<Raw>::initRawMap(Base::valueMap())
		);

		return rawMap;
	}

	static typename EnumHelper<Raw>::NamesMap &namesMap()
	{
		static typename EnumHelper<Raw>::NamesMap namesMap(
			EnumHelper<Raw>::initNamesMap(Base::valueMap())
		);

		return namesMap;
	}

public:
	Raw raw() const
	{
		return m_value->first;
	}

	const std::string &toString() const
	{
		return m_value->second;
	}

	static Enum<Base, Raw> parse(const std::string &input)
	{
		auto it = namesMap().find(input);
		if (it == namesMap().end()) {
			throw Poco::InvalidArgumentException(
				"failed to parse '" + input + "'");
		}

		return Enum<Base, Raw>(it->second);
	}

	static Enum<Base, Raw> fromRaw(const Raw &raw)
	{
		auto it = rawMap().find(raw);
		if (it == rawMap().end()) {
			throw Poco::InvalidArgumentException(
				"unrecognized raw value " + std::to_string(raw));
		}

		return Enum<Base, Raw>(it->second);
	}

	static Enum<Base, Raw> random()
	{
		Poco::Random rnd;
		rnd.seed();

		return fromRaw(rnd.next(rawMap().size()));
	}

	static Enum<Base, Raw> fromRaw(const unsigned int raw)
	{
		return fromRaw(Raw(raw));
	}

	operator Raw() const
	{
		return raw();
	}

	bool operator ==(const Enum &other) const
	{
		return m_value == other.m_value;
	}

	bool operator !=(const Enum &other) const
	{
		return m_value != other.m_value;
	}

	bool operator <(const Enum &other) const
	{
		return m_value->first < other.m_value->first;
	}

	bool operator >(const Enum &other) const
	{
		return m_value->first > other.m_value->first;
	}

	bool operator <=(const Enum &other) const
	{
		return m_value->first <= other.m_value->first;
	}

	bool operator >=(const Enum &other) const
	{
		return m_value->first >= other.m_value->first;
	}

private:
	Value m_value;
};

template <typename Base, typename Raw = typename Base::Raw>
inline std::string operator +(const std::string &s, const Enum<Base, Raw> &e)
{
	return s + e.toString();
}

template <typename Base, typename Raw = typename Base::Raw>
inline std::string operator +(const char *s, const Enum<Base, Raw> &e)
{
	return s + e.toString();
}

}

#endif
