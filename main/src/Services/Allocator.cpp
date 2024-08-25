#include "Allocator.h"
#include <esp_heap_caps.h>
#include <esp_log.h>

static const char* TAG = "Allocator";

Allocator::Allocator(const size_t dataSize) : dataSize(dataSize){
	data = (uint8_t*) heap_caps_malloc(dataSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
	if(data == nullptr){
		ESP_LOGE(TAG, "Failed allocating %zu B", dataSize);
	}
}

Allocator::Allocator(void* buf, size_t dataSize) : dataSize(dataSize), data((uint8_t*) buf), borrowed(true){

}

Allocator::~Allocator(){
	if(!borrowed){
		free(data);
	}
}

void* Allocator::malloc(size_t size){
	if(data == nullptr) return nullptr;
	if(size > freeSize()) return nullptr;

	auto ptr = data + pointer;
	pointer += size;

	return ptr;
}

void Allocator::reset(){
	pointer = 0;
}

size_t Allocator::freeSize(){
	if(data == nullptr) return 0;

	return dataSize - pointer;
}
