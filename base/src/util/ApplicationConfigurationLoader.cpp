#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/File.h>
#include <Poco/Util/Application.h>

#include "util/ApplicationConfigurationLoader.h"

using namespace Poco;
using namespace Poco::Util;
using namespace BeeeOn;

ApplicationConfigurationLoader::ApplicationConfigurationLoader(
		Application &app):
	m_app(app)
{
}

ApplicationConfigurationLoader::~ApplicationConfigurationLoader()
{
}

Application &ApplicationConfigurationLoader::application()
{
	return m_app;
}

void ApplicationConfigurationLoader::load(const Poco::File &file)
{
	if (logger().debug()) {
		logger().debug("loading configuration " + file.path(),
			__FILE__, __LINE__);
	}

	try {
		m_app.loadConfiguration(file.path());

		logger().information("loaded configuration " + file.path(),
				__FILE__, __LINE__);
	} catch (const Exception &e) {
		logger().log(e, __FILE__, __LINE__);
	}
}

void ApplicationConfigurationLoader::finished()
{
}
