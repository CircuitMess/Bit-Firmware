#include "Sleep.h"
#include "Util/stdafx.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

Sleep::Sleep() : Threaded("Sleep", 3 * 1024, 5, 1), evts(6){
	Events::listen(Facility::Input, &evts);
	Events::listen(Facility::Robots, &evts);

	activity = millis();
	start();
}

void Sleep::loop(){
	auto settings = (Settings*) Services.get(Service::Settings);
	const auto set = settings->get();
	if(set.sleepTime == 0 || set.sleepTime >= Settings::SleepSteps){
		vTaskDelay(10000);
		activity = millis();
		return;
	}

	const uint64_t sleepMillis = Settings::SleepSeconds[set.sleepTime] * 1000;
	const auto timeSinceAct = millis() - activity;
	if(timeSinceAct > sleepMillis){ // shouldn't happen
		activity = millis();
		return;
	}

	const auto waitTime = sleepMillis - timeSinceAct;

	Event evt;
	if(!evts.get(evt, waitTime)){
		if(millis() - activity >= sleepMillis){
			off();
			return;
		}else return;
	}

	free(evt.data);
	activity = millis();
}

void Sleep::off(){
	extern void shutdown();
	shutdown();
}
