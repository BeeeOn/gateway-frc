#include <string>

#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/SortedDirectoryIterator.h>
#include <Poco/Util/AbstractConfiguration.h>

#include "util/FileLoader.h"
#include "util/AutoConfigurationExplorer.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace BeeeOn;

AutoConfigurationExplorer::AutoConfigurationExplorer(
		AbstractConfiguration &guide):
	m_guide(guide)
{
}

AutoConfigurationExplorer::~AutoConfigurationExplorer()
{
}

void AutoConfigurationExplorer::handleLink(
		const Poco::File &file,
		FileLoader &)
{
	if (logger().debug()) {
		logger().debug("skipping link " + file.path(),
			__FILE__, __LINE__);
	}
}

void AutoConfigurationExplorer::handleSubDirectory(
		const Poco::File &file,
		FileLoader &)
{
	if (logger().debug()) {
		logger().debug("skipping sub-directory " + file.path(),
			__FILE__, __LINE__);
	}
}

bool AutoConfigurationExplorer::shouldSkip(const Poco::File &file)
{
	return Path(file.path()).getExtension().find("_disable") != string::npos;
}

void AutoConfigurationExplorer::handleFile(
		const Poco::File &file,
		FileLoader &loader)
{
	if (shouldSkip(file)) {
		if (logger().debug()) {
			logger().debug("skipping disabled " + file.path(),
				__FILE__, __LINE__);
		}

		return;
	}

	loader.load(file);
}

void AutoConfigurationExplorer::exploreDirectory(const Poco::Path &path, FileLoader &loader)
{
	const File dir(path);

	if (!path.isDirectory()) {
		if (logger().debug()) {
			logger().debug("path " + dir.path() + " is not a directory",
					__FILE__, __LINE__);
		}

		return;
	}

	if (!dir.exists()) {
		if (logger().debug()) {
			logger().debug("path " + dir.path() + " does not exist",
					__FILE__, __LINE__);
		}

		return;
	}

	SortedDirectoryIterator it(dir);
	SortedDirectoryIterator end;

	if (logger().debug()) {
		logger().debug("exploring directory " + path.toString(),
				__FILE__, __LINE__);
	}

	for(; it != end; ++it) {
		const File current(it.path());
		exploreOne(current, loader);
	}
}

void AutoConfigurationExplorer::exploreOne(const Poco::File &file, FileLoader &loader)
{
	if (!file.exists()) {
		if (logger().debug()) {
			logger().debug("file " + file.path() + " does not exist",
					__FILE__, __LINE__);
		}
	}
	else if (file.isDirectory()) {
		handleSubDirectory(file, loader);
	}
	else if (file.isLink()) {
		handleLink(file, loader);
	}
	else if (file.isFile()) {
		handleFile(file, loader);
	}
	else {
		if (logger().debug()) {
			logger().debug("skip special " + file.path(),
				__FILE__, __LINE__);
		}
	}
}

void AutoConfigurationExplorer::explore(FileLoader &loader)
{
	const string pocoDir = m_guide.getString("application.configDir", "");

	if (logger().debug()) {
		logger().debug("application.configDir = " + pocoDir,
				__FILE__, __LINE__);
	}

	if (!pocoDir.empty()) {
		exploreDirectory(Path(pocoDir).pushDirectory("config.d"), loader);
	}

	loader.finished();
}
