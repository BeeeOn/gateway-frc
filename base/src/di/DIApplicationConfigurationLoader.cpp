#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Path.h>
#include <Poco/Logger.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Poco/DOM/Document.h>

#include "di/DIApplicationConfigurationLoader.h"
#include "di/DIXmlLoader.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace Poco::XML;
using namespace BeeeOn;

DIApplicationConfigurationLoader::DIApplicationConfigurationLoader(Application &app):
	ApplicationConfigurationLoader(app)
{
}

DIApplicationConfigurationLoader::~DIApplicationConfigurationLoader()
{
}

void DIApplicationConfigurationLoader::load(const File &file)
{
	const Path path(file.path());

	if (path.getExtension() != "xml")
		ApplicationConfigurationLoader::load(file);
	else
		loadXml(file);
}

void DIApplicationConfigurationLoader::loadXml(const File &file)
{
	try {
		FileInputStream fin(file.path());

		if (logger().debug()) {
			logger().debug("loading XML configuration " + file.path(),
					__FILE__, __LINE__);
		}

		m_loader.load(fin);
	} catch (const Exception &e) {
		logger().log(e, __FILE__, __LINE__);
		e.rethrow();
	}
}

void DIApplicationConfigurationLoader::finished()
{
	LayeredConfiguration &config = application().config();
	AutoPtr<XMLConfiguration> xml(new XMLConfiguration);

	const Document *doc = m_loader.document();
	if (doc != NULL) {
		if (logger().debug()) {
			logger().debug("loading merged XML configuration",
					__FILE__, __LINE__);
		}

		xml->load(m_loader.document());
		config.add(xml);
	}
}
