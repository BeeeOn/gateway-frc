#pragma once

#include <Poco/AutoPtr.h>
#include <Poco/Util/AbstractConfiguration.h>

#include "util/Loggable.h"

namespace BeeeOn {

class ConfigurationSaver : protected Loggable {
public:
	ConfigurationSaver(const std::string &filePath);
	~ConfigurationSaver();

	Poco::AutoPtr<Poco::Util::AbstractConfiguration> config() const;

	/**
	 * Saves the m_config in the m_file.
	 * m_config is created in constructor based on m_file extension.
	 * Supported extensions: .xml, .json, .property
	 */
	void save() const;

	/**
	 * Saves given configuration in file based on object type.
	 * It does not check the extension of given file name.
	 * Supported types of configuration: XML, JSON, PropertyFile.
	 *
	 * Notice: in the case of XMLConfiguration can throw an exception caused by
	 * the fact, that XMLConfiguration method loadEmpty("root")
	 * was not called before "filling" the configuration.
	 * This issue is fixed in Poco commit 20a6f258, but this commit is not
	 * yet published.
	 */
	static void saveAs(
		Poco::AutoPtr<Poco::Util::AbstractConfiguration> conf,
		const std::string &filePath);

private:
	Poco::AutoPtr<Poco::Util::AbstractConfiguration> m_config;
	std::string m_filePath;
};

}
