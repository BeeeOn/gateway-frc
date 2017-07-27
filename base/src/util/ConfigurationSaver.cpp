#include <typeinfo>
#include <fstream>
#include <string>

#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/Util/JSONConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Util/XMLConfiguration.h>

#include "util/ConfigurationSaver.h"
#include "util/ClassInfo.h"

using namespace BeeeOn;
using namespace Poco;
using namespace Poco::Util;
using namespace std;

ConfigurationSaver::ConfigurationSaver(const string &filePath):
	m_filePath(filePath)
{
	Path path(filePath);

	if (path.getExtension() == "properties") {
		m_config = new PropertyFileConfiguration;
	}
	else if (path.getExtension() == "xml") {
		AutoPtr<XMLConfiguration> config(new XMLConfiguration);
		config->loadEmpty("root");
		m_config = config;
	}
	else if (path.getExtension() == "json") {
		m_config = new JSONConfiguration;
	}
	else {
		throw InvalidArgumentException("unrecognized extension: '"
			+ path.getExtension() + "' in " + path.toString());
	}
}

ConfigurationSaver::~ConfigurationSaver()
{
}

AutoPtr<AbstractConfiguration> ConfigurationSaver::config() const
{
	return m_config;
}

void ConfigurationSaver::save() const
{
	saveAs(m_config, m_filePath);
}

void ConfigurationSaver::saveAs(AutoPtr<AbstractConfiguration> conf, const string &filePath)
{
	if (conf.isNull())
		throw NullPointerException("invalid pointer to AbstractConfiguration");

	if (typeid(*conf.get()) == typeid(PropertyFileConfiguration)) {
		conf.cast<PropertyFileConfiguration>()->save(filePath);
	}
	else if (typeid(*conf.get()) == typeid(JSONConfiguration)) {
		ofstream stream(filePath);
		conf.cast<JSONConfiguration>()->save(stream);
	}
	else if (typeid(*conf.get()) == typeid(XMLConfiguration)) {
		conf.cast<XMLConfiguration>()->save(filePath);
	}
	else {
		throw InvalidArgumentException(
			"unsupported type of configuration: "
			+ ClassInfo::forPointer(conf.get()).name());
	}
}
