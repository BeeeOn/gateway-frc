#pragma once

#include "z-wave/ZWaveMessageFactory.h"

namespace BeeeOn {

class DefaultZWaveMessageFactory : public ZWaveMessageFactory {
public:
	ZWaveMessage::Ptr create(
		uint32_t manufacturer, uint32_t product) override;
};

}
