#include "model/GlobalID.h"

using namespace BeeeOn;

/**
 * Implementation taken from
 *
 *  http://developer.classpath.org/doc/java/util/UUID-source.html
 */
unsigned int GlobalID::hash() const
{
	union {
		char b[16];
		uint32_t u32[4];
	};
	m_uuid.copyTo(b);

	return (u32[0] ^ u32[1]) ^ (u32[2] ^ u32[3]);
}
