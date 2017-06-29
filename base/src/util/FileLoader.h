#ifndef BEEEON_FILE_LOADER_H
#define BEEEON_FILE_LOADER_H

namespace Poco {

class File;

}

namespace BeeeOn {

class FileLoader {
public:
	virtual ~FileLoader();

	virtual void load(const Poco::File &file) = 0;
	virtual void finished() = 0;
};

}

#endif
