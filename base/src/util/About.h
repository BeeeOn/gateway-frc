#ifndef BEEEON_ABOUT_H
#define BEEEON_ABOUT_H

#include <string>

namespace BeeeOn {

struct About {
	unsigned int requirePocoVersion = 0;
	unsigned int recommendPocoVersion = 0;
	std::string version;
	std::string description;
};

}

#endif
