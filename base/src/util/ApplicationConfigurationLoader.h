#ifndef BEEEON_APPLICATION_CONFIGURATION_LOADER_H
#define BEEEON_APPLICATION_CONFIGURATION_LOADER_H

#include "util/FileLoader.h"
#include "util/Loggable.h"

namespace Poco {
namespace Util {

class Application;

}
}

namespace BeeeOn {

class ApplicationConfigurationLoader :
		public FileLoader,
		public Loggable {
public:
	ApplicationConfigurationLoader(Poco::Util::Application &app);
	virtual ~ApplicationConfigurationLoader();

	void load(const Poco::File &file) override;
	void finished() override;

protected:
	Poco::Util::Application &application();

private:
	Poco::Util::Application &m_app;
};

}

#endif
