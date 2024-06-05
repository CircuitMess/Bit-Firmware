#include "NVSUpgrade_1.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

void NVSUpgrade_1::execute() const{
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	__SettingsStruct_Old oldSettings{};
	if(nvs->get("Settings", oldSettings)){
		std::array<uint8_t, (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT> robotsSeen = {0};
		for(size_t i = 0; i < 9; ++i){
			robotsSeen[i] = oldSettings.robotsSeen[i];
		}
		nvs->set("Robots", robotsSeen);

		SettingsStruct newSettings{};
		newSettings.theme = Theme::Theme1;
		newSettings.screenBrightness = oldSettings.screenBrightness;
		newSettings.sleepTime = oldSettings.sleepTime;
		newSettings.sound = oldSettings.sound;
		nvs->set("Settings", newSettings);
	}
}
