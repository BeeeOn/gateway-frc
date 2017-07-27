#include <Poco/Exception.h>

#include "di/Injectable.h"
#include "z-wave/manufacturers/AeotecZWaveMessageFactory.h"
#include "z-wave/products/AeotecZW100ZWaveMessage.h"

BEEEON_OBJECT_BEGIN(BeeeOn, AeotecZWaveMessageFactory)
BEEEON_OBJECT_CASTABLE(ZWaveMessageFactory)
BEEEON_OBJECT_CASTABLE(AeotecZWaveMessageFactory)
BEEEON_OBJECT_TEXT("manufacturer", &AeotecZWaveMessageFactory::setManufacturer)
BEEEON_OBJECT_END(BeeeOn, AeotecZWaveMessageFactory)

using namespace BeeeOn;

ZWaveMessage::Ptr AeotecZWaveMessageFactory::createByProduct(
		const uint32_t product)
{
	switch (product) {
	case AEOTEC_ZW100:
		return new AeotecZW100ZWaveMessage();
	default:
		throw Poco::NotFoundException(
			"unsupported product " + std::to_string(product));
	}
}
