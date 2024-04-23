#ifndef BIT_FIRMWARE_NVSFLASH_H
#define BIT_FIRMWARE_NVSFLASH_H

#include <nvs.h>
#include <string>
#include <esp_log.h>

class NVSFlash {
public:
	NVSFlash();
	virtual ~NVSFlash();

	template<typename T>
	bool get(const std::string& blob, T& data) const{
		size_t out_size = sizeof(T);
		auto err = nvs_get_blob(handle, blob.c_str(), &data, &out_size);

		if(err != ESP_OK){
			ESP_LOGI(TAG, "Blob %s not found", blob.c_str());
			return false;
		}

		return true;
	}

	template<typename T>
	bool set(const std::string& blob, const T& data) const{
		esp_err_t err = nvs_set_blob(handle, blob.c_str(), &data, sizeof(T));

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