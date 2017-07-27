#pragma once

#include "z-wave/ManufacturerZWaveMessageFactory.h"
#include "z-wave/ZWaveMessageFactory.h"
#include "z-wave/products/AeotecZW100ZWaveMessage.h"

namespace BeeeOn {

/*
 * It creates Aeotec products.
 */
class AeotecZWaveMessageFactory : public ManufacturerZWaveMessageFactory {
public:
	ZWaveMessage::Ptr createByProduct(const uint32_t product) override;
};

}
