#include "SystemManager.h"
#include "Util/Services.h"

SystemManager::SystemManager(const std::vector<NVSUpgrade>& upgrades){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	uint32_t flashVersion = 0;
	nvs->get("Version", flashVersion);

	for(const NVSUpgrade& upgrade : upgrades){
		const uint32_t upgradeVersion = upgrade.getVersion();

		if(flashVersion < CurrentVersion){
			if(upgradeVersion > flashVersion && upgradeVersion <= CurrentVersion){
				upgrade.upgrade();
			}
		}
	}

	if(flashVersion > CurrentVersion){
		nvs->set("Version", CurrentVersion);
	}
}