#include <Poco/Hash.h>

#include "model/SimpleID.h"

using namespace BeeeOn;

unsigned int SimpleID::hash() const
{
	return Poco::hash((Poco::Int64) m_value);
}
