#ifndef BIT_FIRMWARE_STORAGE_H
#define BIT_FIRMWARE_STORAGE_H

#include <array>
#include <cstddef>

namespace CharlieGame {

template <typename T, size_t C>
class Storage {
public:
	bool add(T* el){
		if(filled == C) return false;

		for(int i = 0; i < C; i++){
			if(elements[i] == nullptr){
				elements[i] = el;
				filled++;
				return true;
			}
		}

		return false;
	}

	void rem(T* el){
		if(filled == 0) return;

		for(int i = 0; i < C; i++){
			if(elements[i] == el){
				elements[i] = nullptr;
				filled--;
				return;
			}
		}
	}

	size_t count(){ return filled; }

	void iterate(std::function<void(T* el)> fn){
		if(filled == 0) return;

		for(int i = 0; i < C; i++){
			if(elements[i] != nullptr){
				fn(elements[i]);
			}
		}
	}

private:
	std::array<T*, C> elements = { nullptr };
	size_t filled = 0;

};

}


#endif //BIT_FIRMWARE_STORAGE_H
