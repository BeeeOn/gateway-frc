#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/Util/IniFileConfiguration.h>
#include <Poco/Util/JSONConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>

#include "util/ConfigurationLoader.h"

using namespace Poco;
using namespace Poco::Util;
using namespace BeeeOn;

ConfigurationLoader::ConfigurationLoader()
{
}

ConfigurationLoader::~ConfigurationLoader()
{
}

void ConfigurationLoader::load(const Poco::File &file)
{
	load(Path(file.path()));
}

void ConfigurationLoader::load(const Poco::Path &path)
{
	if (logger().debug()) {
		logger().debug("loading configuration " + path.toString(),
				__FILE__, __LINE__);
	}

	if (path.getExtension() == "ini") {
		m_config = new IniFileConfiguration(path.toString());
	}
	else if (path.getExtension() == "properties") {
		m_config = new PropertyFileConfiguration(path.toString());
	}
	else if (path.getExtension() == "xml") {
		m_config = new XMLConfiguration(path.toString());
	}
	else if (path.getExtension() == "json") {
		m_config = new JSONConfiguration(path.toString());
	}
	else {
		throw InvalidArgumentException("unrecognized extension: '"
				+ path.getExtension() + "' in " + path.toString());
	}

	if (logger().debug()) {
		logger().notice("loaded configuration " + path.toString(),
				__FILE__, __LINE__);
	}
}

void ConfigurationLoader::finished()
{
}

AutoPtr<AbstractConfiguration> ConfigurationLoader::config() const
{
	return m_config;
}
