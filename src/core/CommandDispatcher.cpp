#include "core/CommandDispatcher.h"

using namespace BeeeOn;
using namespace Poco;

void CommandDispatcher::registerHandler(SharedPtr<CommandHandler> handler)
{
	for (auto &item : m_commandHandlers) {
		if (item == handler) {
			throw Poco::ExistsException("handler " + handler->name()
				+ " has been registered");
		}
	}

	m_commandHandlers.push_back(handler);
}

void CommandDispatcher::injectImpl(Answer::Ptr answer, SharedPtr<AnswerImpl> impl)
{
	answer->installImpl(impl);
}
