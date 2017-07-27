#include <set>

#include <Poco/Exception.h>

#include "di/DIWrapper.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

DIWDuplicateException::DIWDuplicateException(
		const string &key,
		const type_info &method):
	m_text(string("duplicate property ")
		+ key + " with method " + method.name())
{
}

const char *DIWDuplicateException::what() const noexcept
{
	return m_text.c_str();
}

DIWCastException::DIWCastException(
		const type_info &from,
		const type_info &to):
	m_text(string("could not cast ")
		+ from.name() + " to " + to.name())
{
}

const char *DIWCastException::what() const noexcept
{
	return m_text.c_str();
}

DIWWrongInputException::DIWWrongInputException(const string &text):
	m_text(text)
{
}

const char *DIWWrongInputException::what() const noexcept
{
	return m_text.c_str();
}

DIWMethodHelper::~DIWMethodHelper()
{
}

DIWNumberSetter::~DIWNumberSetter()
{
}

DIWTextSetter::~DIWTextSetter()
{
}

DIWRefSetter::~DIWRefSetter()
{
}

DIWListSetter::~DIWListSetter()
{
}

DIWMapSetter::~DIWMapSetter()
{
}

DIWHook::~DIWHook()
{
}

DIWrapper::~DIWrapper()
{
}

class DIWCastRegistry {
public:
	~DIWCastRegistry()
	{
		for (auto cast : m_cast)
			delete cast;
	}

	void add(DIWCast *cast)
	{
		m_cast.insert(cast);
	}

	set<DIWCast *>::iterator begin()
	{
		return m_cast.begin();
	}

	set<DIWCast *>::iterator end()
	{
		return m_cast.end();
	}

private:
	set<DIWCast *> m_cast;
};

static DIWCastRegistry g_registry;

DIWCast::~DIWCast()
{
}

void DIWCast::add(DIWCast *cast)
{
	g_registry.add(cast);
}

DIWCast *DIWCast::find(const type_info &info, DIWrapper &wrapper)
{
	for (auto cast : g_registry) {
		if (cast->wouldCast(info) && cast->isSame(wrapper))
			return cast;
	}

	return NULL;
}

static map<string, DIWrapperFactory *> &factories()
{
	static map<string, DIWrapperFactory *> registry;
	return registry;
}

void DIWrapperFactory::registerFactory(
		const string &name, DIWrapperFactory &factory)
{
	factories().emplace(name, &factory);
}

DIWrapperFactory &DIWrapperFactory::lookupFactory(const string &name)
{
	auto it = factories().find(name);
	if (it == factories().end())
		throw NotFoundException("factory for class " + name + " is missing");

	return *it->second;
}

void DIWrapperFactory::listFactories(list<string> &names)
{
	for (auto &pair : factories())
		names.push_back(pair.first);
}
