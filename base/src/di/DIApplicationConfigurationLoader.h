#ifndef BEEEON_DI_APPLICATION_CONFIGURATION_LOADER_H
#define BEEEON_DI_APPLICATION_CONFIGURATION_LOADER_H

#include "di/DIXmlLoader.h"
#include "util/ApplicationConfigurationLoader.h"

namespace BeeeOn {

class DIApplicationConfigurationLoader : public ApplicationConfigurationLoader {
public:
	DIApplicationConfigurationLoader(Poco::Util::Application &app);
	virtual ~DIApplicationConfigurationLoader();

	void load(const Poco::File &file) override;
	void finished() override;

protected:
	void loadXml(const Poco::File &file);

private:
	DIXmlLoader m_loader;
};

}

#endif
