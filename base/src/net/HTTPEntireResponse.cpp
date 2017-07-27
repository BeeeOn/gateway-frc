#include <Poco/StreamCopier.h>

#include "net/HTTPEntireResponse.h"

using namespace BeeeOn;
using namespace Poco;
using namespace Poco::Net;
using namespace std;

HTTPEntireResponse::HTTPEntireResponse()
{
}

HTTPEntireResponse::HTTPEntireResponse(HTTPEntireResponse&& other):
	HTTPResponse(other.getVersion(), other.getStatus(), other.getReason()),
	m_body(other.m_body)
{
	setFieldLimit(other.getFieldLimit());
	swap(other); // copy headers
}

HTTPEntireResponse::HTTPEntireResponse(HTTPResponse::HTTPStatus status):
	HTTPResponse(status)
{
}

HTTPEntireResponse::HTTPEntireResponse(HTTPResponse::HTTPStatus status, const string& reason):
	HTTPResponse(status, reason)
{
}

HTTPEntireResponse::HTTPEntireResponse(const string& version, HTTPResponse::HTTPStatus status):
	HTTPResponse(version, status)
{
}

HTTPEntireResponse::HTTPEntireResponse(const string& version, HTTPResponse::HTTPStatus status, const string& reason):
	HTTPResponse(version, status, reason)
{
}

HTTPEntireResponse::~HTTPEntireResponse()
{
}

HTTPEntireResponse& HTTPEntireResponse::operator=(HTTPEntireResponse&& other)
{
	setVersion(other.getVersion());
	setStatus(other.getStatus());
	setReason(other.getReason());
	setFieldLimit(other.getFieldLimit());
	swap(other); // copy headers
	m_body = other.m_body;
	return *this;
}

void HTTPEntireResponse::readBody(istream& istr)
{
	StreamCopier::copyToString(istr, m_body);
}

void HTTPEntireResponse::setBody(const string& body)
{
	m_body = body;
}

string HTTPEntireResponse::getBody() const
{
	return m_body;
}
