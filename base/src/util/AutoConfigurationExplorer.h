#ifndef BEEEON_GUIDED_CONFIGURATION_EXPLORER_H
#define BEEEON_GUIDED_CONFIGURATION_EXPLORER_H

#include <string>

#include "util/ConfigurationExplorer.h"
#include "util/Loggable.h"

namespace Poco {

class File;

namespace Util {

class AbstractConfiguration;

}
}

namespace BeeeOn {

/**
 * Explore configuration files guided by the given configuration.
 * It looks for keys:
 *
 * * application.configDir - in this directory, it processes files in config.d/
 * * application.extended.configDir - processes all files in this directory
 * * application.extended.configFiles - processes files specifies in this list
 *              separated by a platform specific separator (e.g. ':')
 */
class AutoConfigurationExplorer :
		public ConfigurationExplorer,
		Loggable {
public:
	AutoConfigurationExplorer(
		Poco::Util::AbstractConfiguration &guide);
	virtual ~AutoConfigurationExplorer();

	void explore(FileLoader &loader) override;

protected:
	void exploreDirectory(const Poco::Path &path, FileLoader &loader);
	void exploreOne(const Poco::File &file, FileLoader &loader);
	virtual void handleLink(const Poco::File &file, FileLoader &loader);
	virtual void handleSubDirectory(const Poco::File &file, FileLoader &loader);
	virtual void handleFile(const Poco::File &file, FileLoader &loader);
	virtual bool shouldSkip(const Poco::File &file);

private:
	Poco::Util::AbstractConfiguration &m_guide;
};

}

#endif
