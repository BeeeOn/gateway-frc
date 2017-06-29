#ifndef BEEEON_CONFIGURATION_EXPLORER_H
#define BEEEON_CONFIGURATION_EXPLORER_H

namespace BeeeOn {

class FileLoader;

class ConfigurationExplorer {
public:
	virtual ~ConfigurationExplorer();

	virtual void explore(FileLoader &loader) = 0;
};

}

#endif
