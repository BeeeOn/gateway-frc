#ifndef BEEEON_DI_XML_LOADER_H
#define BEEEON_DI_XML_LOADER_H

#include <istream>

#include <Poco/AutoPtr.h>

#include "util/Loggable.h"

namespace Poco {
namespace XML {

class Document;

}
}

namespace BeeeOn {

class DIXmlLoader : Loggable {
public:
	DIXmlLoader();

	void load(std::istream &in);

	const Poco::XML::Document *document() const;

private:
	Poco::AutoPtr<Poco::XML::Document> m_document;
};

}

#endif
