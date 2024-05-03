#ifndef BIT_FIRMWARE_SYSTEM_H
#define BIT_FIRMWARE_SYSTEM_H

#include <cstdint>
#include <sstream>

struct Version {
	uint32_t major = 0;
	uint32_t minor = 0;
	uint32_t commit = 0;

	inline constexpr bool operator == (const Version& other) const{
		return commit == other.commit;
	}

	inline constexpr bool operator < (const Version& other) const{
		return commit < other.commit;
	}

	inline constexpr bool operator > (const Version& other) const{
		return commit > other.commit;
	}

	inline static std::string toString(const Version& version){
		std::stringstream ss;
		ss << version.major << "." << version.minor << "." << version.commit;
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
		ss >> ver.commit;

		return ver;
	}
};

class System {
public:
	inline static constexpr const Version CurrentVersion{.major = 1, .minor = 1, .commit = 88};
};

#endif //BIT_FIRMWARE_SYSTEM_H