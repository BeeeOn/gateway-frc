#ifndef BEEEON_JSON_UTIL_H
#define BEEEON_JSON_UTIL_H

#include <cmath>
#include <string>

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>

namespace BeeeOn {

class JsonUtil {
public:
	/*
	 * Get string from JSON Object.
	 * @param jsonObject Object which contains parsed JSON message.
	 * @parma &key JSON attribute which contains value
	 * @return searched value
	 */
	template <typename T>
	static T extract(
		Poco::JSON::Object::Ptr jsonObject, const std::string &key)
	{
		Poco::Dynamic::Var item;
		item = jsonObject->get(key);

		return item.convert<T>();
	}

	/*
	 * It parses message and return JSON Object.
	 * @param &data JSON message string
	 * @return JSON Object
	 */
	static Poco::JSON::Object::Ptr parse(const std::string &data);
};

template <>
inline double JsonUtil::extract<double>(
	Poco::JSON::Object::Ptr jsonObject, const std::string &key)
{
	if (jsonObject->isNull(key))
		return std::nan("");

	return jsonObject->get(key).convert<double>();
}

}

#endif
