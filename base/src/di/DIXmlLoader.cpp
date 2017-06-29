#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/NodeList.h>

#include "di/DIXmlLoader.h"
#include "util/SecureXmlParser.h"

using namespace std;
using namespace Poco;
using namespace Poco::XML;
using namespace BeeeOn;

DIXmlLoader::DIXmlLoader()
{
}

void DIXmlLoader::load(istream &in)
{
	SecureXmlParser parser;
	Document *doc = parser.parse(in);

	if (doc->documentElement()->localName() != "system") {
		throw InvalidArgumentException("unexpected XML document root element: "
				+ doc->documentElement()->localName());
	}

	Element *factory = doc->documentElement()->getChildElement("factory");
	if (factory == NULL)
		throw InvalidArgumentException("no element factory in the XML file");

	if (m_document.isNull()) {
		logger().debug("parsing first XML document");
		m_document = doc;
	}
	else {
		logger().debug("parsing and merging XML document");

		Element *target = m_document->documentElement()->getChildElement("factory");
		NodeList *children = factory->childNodes();

		for (unsigned long i = 0; i < children->length(); ++i) {
			Node *node = m_document->importNode(children->item(i), true);
			if (node == NULL)
				throw IllegalStateException("imported node is NULL");

			target->appendChild(node);
		}
	}
}

const Document *DIXmlLoader::document() const
{
	return m_document;
}
