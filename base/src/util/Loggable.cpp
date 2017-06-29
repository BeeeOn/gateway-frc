#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>

#include "util/ClassInfo.h"
#include "util/Loggable.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

Loggable::Loggable():
	m_logger(NULL)
{
}

Loggable::Loggable(const ClassInfo &info):
	m_name(info.name()),
	m_logger(NULL)
{
}

Loggable::Loggable(const std::type_info &info):
	Loggable(ClassInfo(info))
{
}

Loggable::~Loggable()
{
}

void Loggable::setupLogger(Logger *logger) const
{
	if (m_logger == NULL) {
		const string name = m_name.empty()?
			ClassInfo::forPointer(this).name() : m_name;

		m_logger = logger == NULL?
			&Poco::Logger::get(name.c_str()) : logger;
	}
}

Logger &Loggable::forMethod(const char *name)
{
	return Logger::get(name);
}

Logger &Loggable::forClass(const ClassInfo &info)
{
	return Logger::get(info.name());
}

Poco::Logger &Loggable::forClass(const type_info &info)
{
	return forClass(ClassInfo(info));
}

void Loggable::configureSimple(
		Logger &logger, const string &level)
{
	logger.setLevel(Logger::parseLevel(level));
	PatternFormatter *formatter = new PatternFormatter;
	formatter->setProperty(PatternFormatter::PROP_PATTERN, "%q %t (%U:%u)");
	logger.setChannel(new FormattingChannel(formatter, new ConsoleChannel));
}
