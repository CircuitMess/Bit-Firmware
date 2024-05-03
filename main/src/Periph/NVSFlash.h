#ifndef BIT_FIRMWARE_NVSFLASH_H
#define BIT_FIRMWARE_NVSFLASH_H

#include <nvs.h>
#include <string>
#include <esp_log.h>
#include <array>
#include <cstring>
#include <functional>
#include "Util/System.h"

class NVSFlash {
public:
	NVSFlash();
	virtual ~NVSFlash();

	template<typename T, typename T_Old = T>
	bool get(const std::string& blob, T& data, std::function<T(const T_Old&)> upgrade = nullptr) const{
		size_t versionSize = 16;
		char version[versionSize];
		memset(version, 0, versionSize);

		esp_err_t err = nvs_get_str(handle, (blob + "_ver").c_str(), version, &versionSize);

		if(upgrade != nullptr && (err != ESP_OK || System::CurrentVersion > Version::fromString(std::string(version)))){
			size_t out_size = sizeof(T_Old);
			T_Old oldData;
			err = nvs_get_blob(handle, blob.c_str(), &oldData, &out_size);

			if(err != ESP_OK){
				ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
				return false;
			}

			data = upgrade(oldData);

			set<T>(blob, data);
		}else{
			size_t out_size = sizeof(T);
			err = nvs_get_blob(handle, blob.c_str(), &data, &out_size);

			if(err != ESP_OK){
				ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
				return false;
			}
		}

		return true;
	}

	template<typename T>
	bool set(const std::string& blob, const T& data) const{
		const std::string ver = Version::toString(System::CurrentVersion);

		char version[16];
		memcpy(version, ver.c_str(), ver.size() + 1);

		esp_err_t err = nvs_set_str(handle, (blob + "_ver").c_str(), version);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store version error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_set_blob(handle, blob.c_str(), &data, sizeof(T));

		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_commit(handle);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s commit error: %d", blob.c_str(), err);
			return false;
		}

		return true;
	}

	template<typename T, size_t S, typename T_Old = T>
	bool get(const std::string& blob, std::array<T, S>& dataArray, std::function<std::array<T, S>(const std::array<T_Old, S>&)> upgrade = nullptr) const requires(S > 0){
		size_t versionSize = 16;
		char version[versionSize];
		memset(version, 0, versionSize);

		esp_err_t err = nvs_get_str(handle, (blob + "_ver").c_str(), version, &versionSize);

		if(upgrade != nullptr && (err != ESP_OK || System::CurrentVersion > Version::fromString(std::string(version)))){
			size_t out_size = sizeof(T_Old) * S;
			std::array<T_Old, S> oldArray;
			err = nvs_get_blob(handle, blob.c_str(), oldArray.data(), &out_size);

			if(err != ESP_OK){
				ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
				return false;
			}

			dataArray = upgrade(oldArray);

			set<T, S, T_Old>(blob, dataArray);
		}else{
			size_t out_size = sizeof(T) * S;
			err = nvs_get_blob(handle, blob.c_str(), dataArray.data(), &out_size);

			if(err != ESP_OK){
				ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
				return false;
			}
		}

		return true;
	}

	template<typename T, size_t S>
	bool set(const std::string& blob, const std::array<T, S>& dataArray) const requires(S > 0){
		const std::string ver = Version::toString(System::CurrentVersion);

		char version[16];
		memcpy(version, ver.c_str(), ver.size() + 1);

		esp_err_t err = nvs_set_str(handle, (blob + "_ver").c_str(), version);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store version error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_set_blob(handle, blob.c_str(), dataArray.data(), sizeof(T) * S);

		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s store error: %d", blob.c_str(), err);
			return false;
		}

		err = nvs_commit(handle);
		if(err != ESP_OK){
			ESP_LOGW(TAG, "NVS blob %s commit error: %d", blob.c_str(), err);
			return false;
		}

		return true;
	}

private:
	static constexpr const char* NVSNamespace = "Bit";
	static constexpr const char* TAG = "NVS";
	nvs_handle_t handle{};
};

#endif //BIT_FIRMWARE_NVSFLASH_H