#include <Poco/Exception.h>

#include "work/AbstractWorkRepository.h"
#include "work/WorkFactory.h"
#include "Debug.h"

using namespace Poco;
using namespace BeeeOn;

AbstractWorkRepository::AbstractWorkRepository()
{
}

void AbstractWorkRepository::store(Work::Ptr work, bool transaction)
{
	const std::string type = WorkFactory::identify(typeid(*work));
	doStore(work, type, transaction);
}

void AbstractWorkRepository::registerFactory(WorkFactory *factory)
{
	if (m_registry.find(factory->id()) == m_registry.end()) {
		m_registry[factory->id()] = factory;
	}
	else {
		throw InvalidArgumentException("factory " + factory->id()
				+ " has been already registered");
	}
}

Work *AbstractWorkRepository::create(
		const std::string &type,
		const WorkID &id)
{
	auto it = m_registry.find(type);
	if (it == m_registry.end())
		throw NotFoundException("no factory for work " + type);

	WorkFactory *factory = it->second;
	Work *work = factory->create();
	work->setId(id);

	return work;
}
