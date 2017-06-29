#include "JsonUtil.h"

using namespace std;
using namespace Poco::JSON;
using namespace BeeeOn;

Object::Ptr JsonUtil::parse(const string &data)
{
	Parser jsonParser;
	jsonParser.parse(data);
	Poco::Dynamic::Var parsedJsonResult = jsonParser.result();

	return parsedJsonResult.extract<Object::Ptr>();
}
