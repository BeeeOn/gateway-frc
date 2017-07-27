#pragma once

#include "z-wave/ZWaveMessageFactory.h"

namespace BeeeOn {

class ManufacturerZWaveMessageFactory : public ZWaveMessageFactory {
public:
	void setManufacturer(const std::string &manufacturer);
	uint32_t manufacturer() const;


	ZWaveMessage::Ptr create(
		const uint32_t manufacturer, const uint32_t product) override;

protected:
	virtual ZWaveMessage::Ptr createByProduct(const uint32_t product) = 0;
private:
	uint32_t m_manufacturer;
};

}
