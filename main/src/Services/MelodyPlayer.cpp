#include "MelodyPlayer.h"
#include "Util/Services.h"
#include "Util/stdafx.h"

MelodyPlayer::MelodyPlayer(uint8_t bpm, std::initializer_list<Tone> tones) : Threaded("MelodyPlayer", 2 * 1024, 12, 1), beatDuration((int)(1000.0f * 60.0f / (float) bpm)), tones(tones){
	audio = (ChirpSystem*) Services.get(Service::Audio);
	beatDuration /= 5.0f;
}

void MelodyPlayer::play(){
	start();
}

bool MelodyPlayer::isPlaying(){
	return running();
}

void IRAM_ATTR MelodyPlayer::loop(){
	static const float MaxPlay = 2000;

	int played = 0;
	while(played < tones.size()){
		std::vector<Tone> queued;
		float queuedDuration = 0;
		for(;;){
			int i = played + queued.size();
			if(i >= tones.size() || (queuedDuration + tones[i].beats * beatDuration) > MaxPlay) break;
			queuedDuration += tones[i].beats * beatDuration;
			queued.push_back(tones[i]);
		}

		played += queued.size();

		const auto chirps = toChirps(queued);
		audio->play(chirps);

		if(queuedDuration < 100){
			queuedDuration = 0;
		}else{
			queuedDuration -= 100;
		}
		delayMillis((int) queuedDuration);
		while(audio->isPlaying()) delayMillis(1);
	}

	stop(0);
}

std::vector<Chirp> MelodyPlayer::toChirps(const std::vector<Tone>& tones){
	std::vector<Chirp> chirps;
	for(const auto& tone : tones){
		chirps.push_back(Chirp { tone.freq, tone.freq, (uint16_t) (tone.beats * beatDuration) });
	}
	return chirps;
}
