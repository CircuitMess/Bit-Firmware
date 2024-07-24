#ifndef PERSE_ROVER_LEDSERVICE_H
#define PERSE_ROVER_LEDSERVICE_H

#include <cstdint>
#include <map>
#include <tuple>
#include <driver/ledc.h>
#include <memory>
#include <mutex>
#include "Util/Threaded.h"
#include "Util/Queue.h"

enum class LED : uint8_t {
	Up, Down, Left, Right, A, B, Menu, RobotCtrl1, RobotCtrl2, RobotCtrl3, RobotCtrl4, RobotCtrl5, RobotCtrl6, COUNT
};

class LEDService : private Threaded {
public:
	explicit LEDService();

	virtual ~LEDService();

	template<typename T, typename... Args>
	void add(LED led, Args&& ... args){
		T* dev = new T(std::forward<Args>(args)...);

		ledDevices[led] = dev;
	}

	void remove(LED led);

	void on(LED led);

	void off(LED led);

	void blink(LED led, uint32_t count = 1, uint32_t period = 1000);

	void breathe(LED led, uint32_t count = 0, uint32_t period = 1000);

	void set(LED led, float percent);

	void breatheTo(LED led, float targetPercent, uint32_t duration = 250);

protected:
	virtual void loop() override;

private:
	enum LEDInstruction {
		On,
		Off,
		Blink,
		Breathe,
		Set,
		BreatheTo
	};

	struct LEDInstructionInfo {
		LED led;
		LEDInstruction instruction;
		uint32_t count;
		uint32_t period;
		float targetPercent;
	};

	std::map<LED, class SingleLED*> ledDevices;
	std::map<LED, std::unique_ptr<class LEDFunction>> ledFunctions;
	Queue<LEDInstructionInfo> instructionQueue;

private:
	void onInternal(LED led);

	void offInternal(LED led);

	void blinkInternal(LED led, uint32_t count, uint32_t period);

	void breatheInternal(LED led, uint32_t count, uint32_t period);

	void setInternal(LED led, float percent);

	void breatheToInternal(LED led, float targetPercent, uint32_t duration);
};

#endif //PERSE_ROVER_LEDSERVICE_H