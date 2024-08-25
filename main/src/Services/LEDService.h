#ifndef BIT_FIRMWARE_LEDSERVICE_H
#define BIT_FIRMWARE_LEDSERVICE_H

#include "Util/Threaded.h"
#include "Util/Events.h"
#include "Devices/Input.h"
#include "Periph/PWM.h"
#include "Util/Queue.h"
#include "RobotManager.h"
#include <hal/ledc_types.h>
#include <memory>

class LEDService : private Threaded {
public:
	LEDService();

	void twinkle();
	void ctrls(Games game);
	void game(Games game);

private:
	enum State { Twinkle, Ctrls, Game } state = Twinkle;

	static constexpr ledc_channel_t FirstFree = LEDC_CHANNEL_3;
	static constexpr int NumChans = LEDC_CHANNEL_MAX - FirstFree;
	std::unordered_map<ledc_channel_t, bool> freeChans;

	struct StateChange {
		State state;
		Games game;
	};
	Queue<StateChange> stateQueue;
	void stateChange(StateChange newState);

	EventQueue evts;

	void loop() override;

	struct TwinkleState {
		uint32_t startTime = 0;
		uint32_t duration;
		std::unique_ptr<PWM> pwm;
		ledc_channel_t chan;
		Input::Button led;
		uint32_t delay;
	};
	std::array<TwinkleState, NumChans> twinkleState;
	void loopTwinkle();
	void newTwinkle(uint8_t i, Input::Button old = (Input::Button) 7, uint32_t delay = 0);

	Games ctrlGame;
	void loopGame();

	void clearLeds();
	void gotoTwinkle();
	void gotoCtrl();

};


#endif //BIT_FIRMWARE_LEDSERVICE_H
