#ifndef BEEEON_LOGGABLE_H
#define BEEEON_LOGGABLE_H

#include <typeinfo>
#include <string>

namespace Poco {

class Logger;

}

namespace BeeeOn {

class ClassInfo;

class Loggable {
public:
	Loggable();
	Loggable(const ClassInfo &info);
	Loggable(const std::type_info &info);
	virtual ~Loggable();

	static Poco::Logger &forMethod(const char *name);
	static Poco::Logger &forClass(const ClassInfo &info);
	static Poco::Logger &forClass(const std::type_info &info);

	template <typename T>
	static Poco::Logger &forInstance(const T *i)
	{
		return forClass(typeid(*i));
	}

	static void configureSimple(
			Poco::Logger &logger, const std::string &level);

protected:
	void setupLogger(Poco::Logger *logger = 0) const;

	Poco::Logger &logger() const
	{
		setupLogger();
		return *m_logger;
	}

private:
	const std::string m_name;
	mutable Poco::Logger *m_logger;
};

}

#endif
