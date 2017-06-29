#include <iostream>

#include <Poco/Environment.h>
#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Version.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/OptionCallback.h>
#include <Poco/Util/HelpFormatter.h>

#include "di/DependencyInjector.h"
#include "di/DIApplicationConfigurationLoader.h"
#include "di/DIDaemon.h"
#include "loop/LoopRunner.h"
#include "util/AutoConfigurationExplorer.h"
#include "util/Loggable.h"
#include "util/PosixSignal.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;
using namespace BeeeOn;

DIDaemon::DIDaemon(const About &about):
	m_about(about)
{
	setUnixOptions(isUnix());

	m_helpOption.shortName("h");
	m_helpOption.fullName("help");
	m_helpOption.required(false);
	m_helpOption.repeatable(false);
	m_helpOption.noArgument();
	m_helpOption.callback(OptionCallback<DIDaemon>(
			this, &DIDaemon::handleHelp));

	m_versionOption.shortName("V");
	m_versionOption.fullName("version");
	m_versionOption.required(false);
	m_versionOption.repeatable(false);
	m_versionOption.noArgument();
	m_versionOption.callback(OptionCallback<DIDaemon>(
			this, &DIDaemon::handleVersion));

	m_debugStartupOption.fullName("debug-startup");
	m_debugStartupOption.required(false);
	m_debugStartupOption.repeatable(false);
	m_debugStartupOption.noArgument();
	m_debugStartupOption.callback(OptionCallback<DIDaemon>(
			this, &DIDaemon::handleDebugStartup));

	m_defineOption.shortName("D");
	m_defineOption.fullName("define");
	m_defineOption.required(false);
	m_defineOption.repeatable(true);
	m_defineOption.argument("<key>=<value>", true);
	m_defineOption.callback(OptionCallback<DIDaemon>(
			this, &DIDaemon::handleDefine));

	m_configOption.shortName("c");
	m_configOption.fullName("config");
	m_configOption.required(false);
	m_configOption.repeatable(false);
	m_configOption.argument("<file>", true);
	m_configOption.callback(OptionCallback<DIDaemon>(
			this, &DIDaemon::handleConfig));

	m_notifyStartedOption.shortName("N");
	m_notifyStartedOption.fullName("notify-started");
	m_notifyStartedOption.required(false);
	m_notifyStartedOption.repeatable(false);
	m_notifyStartedOption.argument("<PID>", true);
	m_notifyStartedOption.binding(
			"di.daemon.notify.started", &config());
}

DIDaemon::~DIDaemon()
{
}

/**
 * Handle all possible uncought throws and print them to stderr as
 * the last emergency action.
 */
int DIDaemon::up(int argc, char **argv, const About &about)
{
	try {
		poco_throw_on_signal;

		DIDaemon daemon(about);
		return daemon.run(argc, argv);
	}
	catch (const Exception &e) {
		cerr << e.displayText() << endl;
	}
	catch (const exception &e) {
		cerr << e.what() << endl;
	}
	catch (const char *s) {
		cerr << s << endl;
	}
	catch (...) {
		cerr << "unknown failure" << endl;
	}

	return EXIT_SOFTWARE;
}

void DIDaemon::initialize(Application &self)
{
	AutoConfigurationExplorer configExplorer(config());
	DIApplicationConfigurationLoader configLoader(*this);
	configExplorer.explore(configLoader);

	Application::initialize(self);
}

int DIDaemon::main(const std::vector<std::string> &)
{
	if (helpRequested()) {
		printHelp();
		return EXIT_OK;
	}
	if (versionRequested()) {
		printVersion();
		return EXIT_OK;
	}

	logStartup();

	testPocoCompatibility();

	if (logger().debug())
		ManifestSingleton::reportInfo(logger());

	logger().debug("creating runner " + runnerName(),
			__FILE__, __LINE__);

	try {
		startRunner(runnerName());
		return EXIT_OK;
	}
	catch (const Exception &e) {
		logger().critical(e.displayText(), __FILE__, __LINE__);
	}
	catch (const exception &e) {
		logger().critical(e.what(), __FILE__, __LINE__);
	}
	catch (const char *s) {
		logger().critical(s, __FILE__, __LINE__);
	}
	catch (...) {
		logger().critical("unknown failure", __FILE__, __LINE__);
	}

	return EXIT_SOFTWARE;
}

void DIDaemon::startRunner(const string &name)
{
	DependencyInjector di(config().createView("factory"));
	SharedPtr<LoopRunner> runner = di.create<LoopRunner>(name);

	logger().notice("starting runner " + name,
			__FILE__, __LINE__);

	runner->start();

	try {
		notifyStarted();

		waitForTerminationRequest();
		runner->stop();
	} catch (...) {
		runner->stop();
		throw;
	}
}

void DIDaemon::defineOptions(OptionSet &options)
{
	options.addOption(m_helpOption);
	options.addOption(m_versionOption);
	options.addOption(m_debugStartupOption);
	options.addOption(m_defineOption);
	options.addOption(m_configOption);
	options.addOption(m_notifyStartedOption);
}

void DIDaemon::handleHelp(const string &, const string &)
{
	stopOptionsProcessing();
	m_helpRequested = true;
}

void DIDaemon::printHelp() const
{
	HelpFormatter formatter(options());
	formatter.setCommand(config().getString("application.baseName"));
	formatter.setUnixStyle(isUnix());
	formatter.setWidth(80);
	formatter.setUsage("[-h] ...");

	if (!m_about.description.empty())
		formatter.setHeader(m_about.description);

	formatter.format(cout);
}

void DIDaemon::handleVersion(const string &, const string &)
{
	stopOptionsProcessing();
	m_versionRequested = true;
}

void DIDaemon::printVersion() const
{
	cout << version() << endl;
}

void DIDaemon::handleDebugStartup(const string &, const string &)
{
	Loggable::configureSimple(Logger::root(), "debug");
}

void DIDaemon::handleDefine(const string &, const string &value)
{
	size_t off = value.find("=");
	if (off == string::npos) {
		logger().debug("overriding " + value + " as empty",
				__FILE__, __LINE__);
		config().setString(value, "");
	}
	else {
		const string key = value.substr(0, off);
		const string val = value.substr(off + 1);

		logger().debug("overriding " + key + " = " + val,
				__FILE__, __LINE__);
		config().setString(key, val);
	}
}

void DIDaemon::handleConfig(const string &, const string &value)
{
	logger().debug("loading configuration: " + value,
			__FILE__, __LINE__);

	loadConfiguration(value);

	Path configDir(value);
	if (configDir.isAbsolute())
		config().setString("application.configDir", configDir.parent().toString());
	else
		config().setString("application.configDir", configDir.absolute().parent().toString());
}

bool DIDaemon::isUnix() const
{
#if defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION) || defined(__APPLE__)
	return true;
#else
	return false;
#endif
}

bool DIDaemon::helpRequested() const
{
	return m_helpRequested;
}

bool DIDaemon::versionRequested() const
{
	return m_versionRequested;
}

string DIDaemon::runnerName()
{
	return config().getString("application.di.runner", "main");
}

string DIDaemon::version() const
{
	if (m_about.version.empty())
		return "unknown";

	return m_about.version;
}

static string pocoVersion(unsigned long version = 0)
{
	version = version == 0? Environment::libraryVersion() : version;

	unsigned int major = (version >> 24) & 0xff;
	unsigned int minor = (version >> 16) & 0xff;
	unsigned int alpha = (version >>  8) & 0xff;
	unsigned int  beta = (version >>  0) & 0xff;

	return to_string(major) + "." + to_string(minor) + "." + to_string(alpha) + "-" + to_string(beta);
}

void DIDaemon::testPocoCompatibility() const
{
	bool upgrade = false;

	if (Environment::libraryVersion() > POCO_VERSION) {
		logger().warning(
			"runtime Poco library is newer then built-in headers",
			__FILE__, __LINE__);
	}

	if (Environment::libraryVersion() < m_about.requirePocoVersion) {
		throw IllegalStateException("too old Poco library, required at least "
			+ pocoVersion(m_about.requirePocoVersion));
	}

	if (POCO_VERSION < m_about.recommendPocoVersion) {
		logger().warning(
			"Poco library headers are older then recommended",
			__FILE__, __LINE__);
		upgrade = true;
	}

	if (Environment::libraryVersion() < m_about.recommendPocoVersion) {
		logger().warning(
			"runtime Poco library is older then recommended",
			__FILE__, __LINE__);
		upgrade = true;
	}

	if (upgrade) {
		logger().warning("recommended to upgrade Poco library to version "
			+ pocoVersion(m_about.recommendPocoVersion)
			+ " or newer",
			__FILE__, __LINE__);
	}
}

void DIDaemon::logStartup() const
{
	logger().notice("version " + version(), __FILE__, __LINE__);

	logger().notice("Poco library "
		+ pocoVersion()
		+ " (headers " + pocoVersion(POCO_VERSION) + ")",
		__FILE__, __LINE__);

	logger().notice("OS "
		+ Environment::osDisplayName()
		+ " (" + Environment::osName() + " " + Environment::osVersion() + ")",
		__FILE__, __LINE__);

	logger().notice("Machine "
		+ Environment::osArchitecture()
		+ " (cores: " + to_string(Environment::processorCount()) + ")",
		__FILE__, __LINE__);

	logger().debug("Node "
		+ Environment::nodeName()
		+ " (" + Environment::nodeId() + ")",
		__FILE__, __LINE__);
}

void DIDaemon::notifyStarted() const
{
	int pid = config().getInt("di.daemon.notify.started", -1);
	if (pid < 0)
		return;

	try {
		PosixSignal::send(pid, "SIGTERM");
		logger().debug("started, notify process "
			+ to_string(pid), __FILE__, __LINE__);
	} catch (const Exception &e) {
		logger().log(e, __FILE__, __LINE__);
	}
}
