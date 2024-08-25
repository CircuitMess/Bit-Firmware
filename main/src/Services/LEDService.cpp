#include "LEDService.h"
#include "Util/stdafx.h"
#include "Pins.hpp"
#include "RobotManager.h"
#include <esp_random.h>
#include <cmath>

static const std::unordered_map<Input::Button, gpio_num_t> PinMap = {
		{ Input::Button::A, (gpio_num_t) LED_A },
		{ Input::Button::B, (gpio_num_t) LED_B },
		{ Input::Button::Menu, (gpio_num_t) LED_MENU },
		{ Input::Button::Up, (gpio_num_t) LED_UP },
		{ Input::Button::Down, (gpio_num_t) LED_DOWN },
		{ Input::Button::Left, (gpio_num_t) LED_LEFT },
		{ Input::Button::Right, (gpio_num_t) LED_RIGHT },
};

#define BtnA Input::Button::A
#define LR Input::Button::Left, Input::Button::Right
#define UD Input::Button::Up, Input::Button::Down
#define DPAD Input::Button::Down, Input::Button::Up, Input::Button::Left, Input::Button::Right
#define ALL DPAD, BtnA

static const std::unordered_map<Games, std::unordered_set<Input::Button>> GameButtons = {
		{ Games::WackyStacky, { BtnA } },
		{ Games::Blocks, { ALL } },
		{ Games::Snake, { DPAD } },
		{ Games::Pong, { UD } },
		{ Games::Artemis, { ALL } },
		{ Games::MrBee, { BtnA } },
		{ Games::Bob, { LR } },
		{ Games::Buttons, { Input::Button::Up, LR } },
		{ Games::Capacitron, { LR } },
		{ Games::Hertz, { BtnA } },
		{ Games::Marv, { UD } },
		{ Games::Resistron, { LR, BtnA } },
		{ Games::Robby, { LR, BtnA } },
		{ Games::Harald, { DPAD } },
		{ Games::Charlie, { ALL } },
		{ Games::Planck, { LR, BtnA, Input::Button::B } },
		{ Games::Dusty, { BtnA } },
		{ Games::Sparkly, { LR, BtnA, Input::Button::B } }
};

LEDService::LEDService() : Threaded("LED2", 4096, 5, 1), stateQueue(6), evts(6){
	for(int i = FirstFree; i < LEDC_CHANNEL_MAX; i++){
		freeChans.emplace((ledc_channel_t) i, true);
	}

	clearLeds();
	gotoTwinkle();
	start();
}

void LEDService::twinkle(){
	StateChange state { Twinkle, Games::COUNT };
	stateQueue.post(state, 0);
}

void LEDService::ctrls(Games game){
	StateChange state { Ctrls, game };
	stateQueue.post(state, 0);
}

void LEDService::game(Games game){
	StateChange state { Game, game };
	stateQueue.post(state, 0);
}

void LEDService::stateChange(StateChange newState){
	if(newState.state == Twinkle){
		gotoTwinkle();
		state = Twinkle;
	}else if(newState.state == Game || newState.state == Ctrls){
		ctrlGame = newState.game;
		gotoCtrl();
		state = newState.state;
	}

	Events::unlisten(&evts);
	evts.reset();
	if(newState.state == Game){
		Events::listen(Facility::Input, &evts);
	}
}

void LEDService::loop(){
	StateChange newState = {};
	bool changed = false;
	while(stateQueue.get(newState, 0)){
		changed = true;
	}
	if(changed && newState.state != state){
		stateChange(newState);
	}

	if(state == Twinkle){
		loopTwinkle();
	}else if(state == Game){
		loopGame();
	}

	delayMillis(10);
}

void LEDService::loopTwinkle(){
	for(int i = 0; i < NumChans; i++){
		auto& twinkle = twinkleState[i];

		if(twinkle.startTime == 0){
			newTwinkle(i);
		}else if(twinkle.delay != 0){
			if(millis() >= twinkle.startTime + twinkle.delay){
				twinkle.delay = 0;
				twinkle.startTime = millis();
			}
		}else if(millis() - twinkle.startTime > twinkle.duration){
			freeChans[twinkle.chan] = true;
			const auto old = twinkle.led;

			twinkle.pwm->setDuty(0);
			twinkle.pwm->detach();
			twinkle = {};

			newTwinkle(i, old);
		}else{
			float t = std::min(1.0f, (float) (millis() - twinkle.startTime) / (float) twinkle.duration);
			if(t < 0.5f){
				t *= 2.0f;
			}else{
				t = 1.0f - (t - 0.5f) * 2.0f;
			}

			twinkle.pwm->setDuty((int) std::round(t * 10.0f));
		}
	}
}

void LEDService::newTwinkle(uint8_t i, Input::Button old, uint32_t delay){
	std::unordered_set<Input::Button> freeLeds = {
			Input::Button::A, Input::Button::B, Input::Button::Menu,
			Input::Button::Left, Input::Button::Right, Input::Button::Up, Input::Button::Down
	};
	freeLeds.erase(old);

	for(int i = 0; i < NumChans; i++){
		if(twinkleState[i].startTime == 0) continue;
		freeLeds.erase(twinkleState[i].led);
	}

	if(freeLeds.empty()) return;

	std::vector<Input::Button> freeLedsVec(freeLeds.cbegin(), freeLeds.cend());
	const auto led = freeLedsVec[esp_random() % freeLedsVec.size()];

	ledc_channel_t chan = LEDC_CHANNEL_MAX;
	for(int i = FirstFree; i < LEDC_CHANNEL_MAX; i++){
		if(!freeChans[(ledc_channel_t) i]) continue;
		chan = (ledc_channel_t) i;
	}

	if(chan == LEDC_CHANNEL_MAX) return;

	auto pwm = new PWM(PinMap.at(led), chan, false);
	freeChans[chan] = false;

	pwm->attach();
	pwm->setDuty(0);

	const auto duration = 3000 + (esp_random() % 2000);

	twinkleState[i] = { (uint32_t) millis(), duration, std::unique_ptr<PWM>(pwm), chan, led, delay };
}

void LEDService::loopGame(){
	Event evt{};
	if(!evts.get(evt, 0)) return;
	if(evt.facility != Facility::Input){
		free(evt.data);
		return;
	}

	auto data = (Input::Data*) evt.data;

	for(int i = 0; i < NumChans; i++){
		if(twinkleState[i].startTime == 0 || twinkleState[i].led != data->btn) continue;
		twinkleState[i].pwm->setDuty(data->action == Input::Data::Press ? 0 : 10);
	}


	free(evt.data);
}

void LEDService::clearLeds(){
	for(int i = 0; i < NumChans; i++){
		if(twinkleState[i].startTime == 0) continue;
		twinkleState[i].pwm->setDuty(0);
		twinkleState[i].pwm->detach();
		twinkleState[i] = {};
	}

	for(int i = FirstFree; i < LEDC_CHANNEL_MAX; i++){
		freeChans[(ledc_channel_t) i] = true;
	}

	for(const auto& map : PinMap){
		gpio_config_t cfg = {
				.pin_bit_mask = 1ULL << map.second,
				.mode = GPIO_MODE_OUTPUT
		};
		gpio_config(&cfg);
		gpio_set_level(map.second, 0);
	}
}

void LEDService::gotoTwinkle(){
	clearLeds();

	for(int i = 0; i < NumChans; i++){
		newTwinkle(i, (Input::Button) 7, esp_random() % 1000);
	}
}

void LEDService::gotoCtrl(){
	clearLeds();

	const auto& ctrlLeds = GameButtons.at(ctrlGame);

	int i = 0;
	for(const auto& led : ctrlLeds){
		auto chan = (ledc_channel_t) (FirstFree + i);

		auto pwm = new PWM(PinMap.at(led), chan);
		pwm->attach();
		pwm->setDuty(10);

		freeChans[chan] = false;
		twinkleState[i] = { (uint32_t) millis(), 0, std::unique_ptr<PWM>(pwm), chan, led, 0 };

		if(++i >= NumChans) break;
	}
}
