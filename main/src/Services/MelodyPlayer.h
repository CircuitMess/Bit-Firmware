#ifndef BIT_FIRMWARE_MELODYPLAYER_H
#define BIT_FIRMWARE_MELODYPLAYER_H

#include <cstdint>
#include <vector>
#include <initializer_list>
#include "Util/Threaded.h"
#include "ChirpSystem.h"

struct Tone {
	uint16_t freq = 0;
	float beats = 0;
};

class MelodyPlayer : private Threaded {
public:
	MelodyPlayer(uint8_t bpm, std::initializer_list<Tone> tones);

	void play();
	bool isPlaying();

private:
	ChirpSystem* audio;

	float beatDuration; // [ms]
	std::vector<Tone> tones;

	void loop() override;

	std::vector<Chirp> toChirps(const std::vector<Tone>& tones);

};


#endif //BIT_FIRMWARE_MELODYPLAYER_H
