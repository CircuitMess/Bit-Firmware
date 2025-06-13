#include "Pins.hpp"
#include "Util/EfuseMeta.h"

static const char* TAG = "Pins";

int Pins::get(Pin pin){

	if(!instance){
		instance = new Pins();

		uint8_t revision = 0;
		EfuseMeta::readRev(revision);

		if(revision == 0 || revision == 1 || revision == 2){
			instance->currentMap = &instance->Revision1;
		}else if(revision == 3){
			instance->currentMap = &instance->Revision3;
		}else{
			while(true){
				EfuseMeta::log();
				while(true);
			}
		}
	}

	assert(instance != nullptr);

	PinMap* pinMap = instance->currentMap;

	if(!pinMap->contains(pin)){
		ESP_LOGE(TAG, "Pin %d not mapped!\n", (int)pin);
		return -1;
	}

	return pinMap->at(pin);
}

void Pins::setLatest(){
	if(!instance){
		instance = new Pins();
	}

	instance->currentMap = instance->pinMaps.back();
}

Pins::Pins(){
	initPinMaps();
}

void Pins::initPinMaps(){
	Revision1 = {
			{ Pin::BtnDown,  4 },
			{ Pin::BtnUp,    5 },
			{ Pin::BtnLeft,  3 },
			{ Pin::BtnRight, 6 },
			{ Pin::BtnA,     7 },
			{ Pin::BtnB,     8 },
			{ Pin::BtnMenu,  9 },
			{ Pin::LedBl,    12 },
			{ Pin::Buzz,     11 },
			{ Pin::BattRead, 10 },
			{ Pin::BattVref, -1 },
			{ Pin::I2cSda,   1 },
			{ Pin::I2cScl,   2 },
			{ Pin::TftSck,   13 },
			{ Pin::TftMosi,  14 },
			{ Pin::TftDc,    15 },
			{ Pin::TftRst,   16 },
			{ Pin::Det1,     44 },
			{ Pin::Det2,     21 },
			{ Pin::Addr1,    43 },
			{ Pin::Addr2,    42 },
			{ Pin::Addr3,    41 },
			{ Pin::Addr4,    40 },
			{ Pin::Addr5,    39 },
			{ Pin::Addr6,    38 },
			{ Pin::Ctrl1,    37 },
			{ Pin::Ctrl2,    36 },
			{ Pin::Ctrl3,    35 },
			{ Pin::Ctrl4,    34 },
			{ Pin::Ctrl5,    33 },
			{ Pin::Ctrl6,    26 },
			{ Pin::LedDown,  48 },
			{ Pin::LedUp,    47 },
			{ Pin::LedLeft,  0 },
			{ Pin::LedRight, 18 },
			{ Pin::LedA,     17 },
			{ Pin::LedB,     45 },
			{ Pin::LedMenu,  46 },
	};

	Revision3 = {
			{ Pin::BtnDown,  13 },
			{ Pin::BtnUp,    11 },
			{ Pin::BtnLeft,  12 },
			{ Pin::BtnRight, 9 },
			{ Pin::BtnA,     14 },
			{ Pin::BtnB,     15 },
			{ Pin::BtnMenu,  21 },
			{ Pin::LedBl,    8 },
			{ Pin::Buzz,     16 },
			{ Pin::BattRead, 3 },
			{ Pin::BattVref, 2 },
			{ Pin::I2cSda,   1 },
			{ Pin::I2cScl,   0 },
			{ Pin::TftSck,   4 },
			{ Pin::TftMosi,  5 },
			{ Pin::TftDc,    6 },
			{ Pin::TftRst,   7 },
			{ Pin::Det1,     44 },
			{ Pin::Det2,     47 },
			{ Pin::Addr1,    43 },
			{ Pin::Addr2,    42 },
			{ Pin::Addr3,    41 },
			{ Pin::Addr4,    40 },
			{ Pin::Addr5,    39 },
			{ Pin::Addr6,    38 },
			{ Pin::Ctrl1,    37 },
			{ Pin::Ctrl2,    36 },
			{ Pin::Ctrl3,    35 },
			{ Pin::Ctrl4,    34 },
			{ Pin::Ctrl5,    33 },
			{ Pin::Ctrl6,    10 },
			{ Pin::LedDown,  -1 },
			{ Pin::LedUp,    -1 },
			{ Pin::LedLeft,  -1 },
			{ Pin::LedRight, -1 },
			{ Pin::LedA,     -1 },
			{ Pin::LedB,     -1 },
			{ Pin::LedMenu,  -1 },
	};
}

