#ifndef BIT_FIRMWARE_SYSTEM_H
#define BIT_FIRMWARE_SYSTEM_H

#include <cstdint>
#include <sstream>

struct Version {
	uint32_t major = 0;
	uint32_t minor = 0;
	uint32_t changeset = 0;

	inline constexpr bool operator == (const Version& other) const{
		return changeset == other.changeset;
	}

	inline constexpr bool operator < (const Version& other) const{
		return changeset < other.changeset;
	}

	inline constexpr bool operator > (const Version& other) const{
		return changeset > other.changeset;
	}

	inline static std::string toString(const Version& version){
		std::stringstream ss;
		ss << version.major << "." << version.minor << "." << version.changeset;
		return ss.str();
	}

	inline static Version fromString(const std::string& version){
		char temp;
		Version ver;

		std::stringstream ss(version);
		ss >> ver.major;
		ss.get(temp);
		ss >> ver.minor;
		ss.get(temp);
		ss >> ver.changeset;

		return ver;
	}
};

class System {
public:
	inline static constexpr const Version CurrentVersion{.major = 1, .minor = 1, .changeset = 88};
	inline static constexpr const size_t VersionStringSize = 16;
};

#endif //BIT_FIRMWARE_SYSTEM_H