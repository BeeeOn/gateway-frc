#include <Poco/SingletonHolder.h>

#include "work/WorkRepository.h"

using namespace Poco;
using namespace BeeeOn;

WorkRepository::~WorkRepository()
{
}

void EmptyWorkRepository::store(Work::Ptr, bool)
{
}

void EmptyWorkRepository::loadScheduled(std::vector<Work::Ptr> &)
{
}

WorkRepository &EmptyWorkRepository::instance()
{
	static SingletonHolder<EmptyWorkRepository> singleton;
	return *singleton.get();
}
