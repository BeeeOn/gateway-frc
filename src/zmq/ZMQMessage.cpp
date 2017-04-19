#include "util/JsonUtil.h"
#include "zmq/ZMQMessage.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

ZMQMessage::ZMQMessage() :
	m_json(new JSON::Object)
{
}

ZMQMessage::ZMQMessage(JSON::Object::Ptr jsonObject)
{
	m_json = jsonObject;
}

ZMQMessageType ZMQMessage::type() const
{
	return ZMQMessageType::parse(
		JsonUtil::extract<string>(m_json, "message_type")
	);
}

std::string ZMQMessage::toString() const
{
	return Dynamic::Var::toString(m_json);
}

void ZMQMessage::setType(const ZMQMessageType &type)
{
	m_json->set("message_type", type.toString());
}

void ZMQMessage::setErrorCode(const ZMQMessageError::Error &error)
{
	m_json->set("error_code", (int) error);
}

void ZMQMessage::setErrorMessage(const string errorMessage)
{
	m_json->set("error_message", errorMessage);
}

ZMQMessageError::Error ZMQMessage::getErrorCode()
{
	return static_cast<ZMQMessageError::Error>(
		JsonUtil::extract<int>(m_json, "error_code"));
}

std::string ZMQMessage::getErrorMessage()
{
	return JsonUtil::extract<string>(m_json, "error_message");
}

ZMQMessage ZMQMessage::fromError(
	const ZMQMessageError::Error error, const std::string &message)
{
	ZMQMessage msg;

	msg.setType(ZMQMessageType::fromRaw(
		ZMQMessageType::TYPE_ERROR));
	msg.setErrorCode(error);
	msg.setErrorMessage(message);

	return msg;
}

ZMQMessage ZMQMessage::fromJSON(const string &json)
{
	return ZMQMessage(JsonUtil::parse(json));
}

ZMQMessageError ZMQMessage::toError()
{
	return ZMQMessageError(getErrorCode(), getErrorMessage());
}