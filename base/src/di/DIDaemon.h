#ifndef BEEEON_DI_DAEMON_H
#define BEEEON_DI_DAEMON_H

#include <string>

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>

#include "util/About.h"

namespace Poco {
namespace Util {

class OptionSet;

}
}

namespace BeeeOn {

class DIDaemon : public Poco::Util::ServerApplication {
public:
	DIDaemon(const About &about);
	virtual ~DIDaemon();

	static int up(int argc, char **argv,
		const About &about = About());

protected:
	void initialize(Poco::Util::Application &self) override;
	int main(const std::vector<std::string> &args) override;
	void defineOptions(Poco::Util::OptionSet &options) override;
	void handleHelp(const std::string &name, const std::string &value);
	void handleVersion(const std::string &name, const std::string &value);
	void handleDebugStartup(const std::string &name, const std::string &value);
	void handleDefine(const std::string &name, const std::string &value);
	void handleConfig(const std::string &name, const std::string &value);
	void startRunner(const std::string &name);
	void printHelp() const;
	void printVersion() const;
	void notifyStarted() const;
	void testPocoCompatibility() const;
	void logStartup() const;
	bool isUnix() const;
	bool helpRequested() const;
	bool versionRequested() const;
	std::string runnerName();
	std::string version() const;

private:
	bool m_helpRequested = false;
	bool m_versionRequested = false;
	About m_about;
	Poco::Util::Option m_helpOption;
	Poco::Util::Option m_versionOption;
	Poco::Util::Option m_debugStartupOption;
	Poco::Util::Option m_defineOption;
	Poco::Util::Option m_configOption;
	Poco::Util::Option m_notifyStartedOption;
};

}

#endif
