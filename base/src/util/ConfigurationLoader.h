#ifndef BEEEON_CONFIGURATION_LOADER_H
#define BEEEON_CONFIGURATION_LOADER_H

#include <Poco/Util/AbstractConfiguration.h>

#include "util/FileLoader.h"
#include "util/Loggable.h"

namespace Poco {

class File;
class Path;

}

namespace BeeeOn {

class ConfigurationLoader : public FileLoader, public Loggable {
public:
	ConfigurationLoader();
	virtual ~ConfigurationLoader();

	void load(const Poco::File &file) override;
	virtual void load(const Poco::Path &path);

	void finished() override;

	Poco::AutoPtr<Poco::Util::AbstractConfiguration> config() const;

private:
	Poco::AutoPtr<Poco::Util::AbstractConfiguration> m_config;
};

}

#endif
