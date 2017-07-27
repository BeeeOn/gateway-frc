#include "di/Injectable.h"
#include "z-wave/manufacturers/DefaultZWaveMessageFactory.h"
#include "z-wave/products/DefaultZWaveMessage.h"

BEEEON_OBJECT_BEGIN(BeeeOn, DefaultZWaveMessageFactory)
BEEEON_OBJECT_CASTABLE(ZWaveMessageFactory)
BEEEON_OBJECT_CASTABLE(DefaultZWaveMessageFactory)
BEEEON_OBJECT_END(BeeeOn, DefaultZWaveMessageFactory)

using namespace BeeeOn;

ZWaveMessage::Ptr DefaultZWaveMessageFactory::create(uint32_t, uint32_t)
{
	return new DefaultZWaveMessage();
}
