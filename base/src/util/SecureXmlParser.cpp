#include <Poco/Exception.h>
#include <Poco/DOM/DOMBuilder.h>
#include <Poco/SAX/DeclHandler.h>
#include <Poco/SAX/WhitespaceFilter.h>
#include <Poco/SAX/LexicalHandler.h>
#include <Poco/SAX/InputSource.h>

#include "util/SecureXmlParser.h"

using namespace std;
using namespace Poco;
using namespace Poco::XML;
using namespace BeeeOn;

namespace BeeeOn {

class DenyDTDHandler : public LexicalHandler {
public:
	void comment(const XMLChar[], int, int)
	{
	}

	void startCDATA()
	{
	}

	void endCDATA()
	{
	}

	/**
	 * Throws InvalidArgumentException when called.
	 */
	void startDTD(
		const XMLString &,
		const XMLString &,
		const XMLString &)
	{
		throw InvalidArgumentException(
				"DTD is forbidden for this parser");
	}

	void endDTD()
	{
	}

	void startEntity(const XMLString &)
	{
	}

	void endEntity(const XMLString &)
	{
	}
};

}

SecureXmlParser::SecureXmlParser()
{
}

SecureXmlParser::~SecureXmlParser()
{
}

void SecureXmlParser::secure(Poco::XML::XMLReader &reader, DenyDTDHandler &handler)
{
	reader.setFeature(XMLReader::FEATURE_NAMESPACES, true);
	reader.setFeature(XMLReader::FEATURE_NAMESPACE_PREFIXES, true);

	reader.setProperty(XMLReader::PROPERTY_LEXICAL_HANDLER,
		static_cast<Poco::XML::LexicalHandler*>(&handler));
}

Document *SecureXmlParser::parse(const char *input, std::size_t length)
{
	SAXParser parser;
	WhitespaceFilter filter(&parser);
	DenyDTDHandler lexicalHandler;

	DOMBuilder builder(filter);
	secure(filter, lexicalHandler);

	return builder.parseMemoryNP(input, length);
}

Document *SecureXmlParser::parse(const string &input)
{
	return parse(input.c_str(), input.size());
}

Document *SecureXmlParser::parse(std::istream &in)
{
	SAXParser parser;
	WhitespaceFilter filter(&parser);
	DenyDTDHandler lexicalHandler;

	InputSource source(in);

	DOMBuilder builder(filter);
	secure(filter, lexicalHandler);
	return builder.parse(&source);
}
