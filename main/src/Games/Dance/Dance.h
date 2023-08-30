#ifndef CIRCUITPET_FIRMWARE_GAME5_H
#define CIRCUITPET_FIRMWARE_GAME5_H

#include "GameEngine/Game.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Games/Common/Hearts.h"
#include <deque>

class Dance : public Game {
public:
	Dance(Sprite& base);

protected:
	void onStart() override;
	void onStop() override;
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

private:
	enum {
		Running, DoneAnim, DonePause
	} state = Running;

	GameObject bottomWall;
	GameObjPtr scoreBar;
	std::shared_ptr<Sprite> scoreBarSprite;
	std::unique_ptr<Hearts> hearts;

	GameObjPtr bars[3];
	GameObjPtr circles[3];
	GameObjPtr player;
	std::shared_ptr<AnimRC> duckRC;

	void gameDone(bool success);
	constexpr static float gameDonePause = 1.5f;
	float gameDoneTimer = 0;

	uint8_t life = 3;
	uint32_t score = 0;
	constexpr static uint32_t perfectBonus = 200;
	constexpr static uint32_t notePoints = 100;
	constexpr static uint32_t goal = 15 * (perfectBonus + notePoints);
	float beatTimer = 0;
	constexpr static float defaultBeatInterval = 1.0f;
	float beatInterval = defaultBeatInterval;

	constexpr static float failDuration = 0.8f;
	float failTime[3] = { 0 };
	constexpr static float failBlinkDuration = 0.2f;
	bool fail[3] = { 0 };

	constexpr static int16_t barsY = 10;
	constexpr static uint16_t barsX[3] = { 4, 20, 36 };
	constexpr static const char* barsIcons[4] = { "/BarP.raw", "/BarY.raw", "/BarB.raw", "/BarR.raw" };

	bool circlesPressed[3] = { false, false, false };
	constexpr static uint16_t circlesY = 104;
	constexpr static uint16_t circlesX[3] = { 2, 18, 34 };
	constexpr static const char* circlesIcons[3] = { "/circ-p1.raw", "/circ-y1.raw", "/circ-b1.raw" };
	constexpr static const char* circlesIconsPressed[3] = { "/circ-p2.raw", "/circ-y2.raw", "/circ-b2.raw" };


	std::deque<GameObjPtr> notes[3];
	constexpr static const char* notesIcon = "/tocka.raw";
	constexpr static const char* danceGIFs[3] = { "/dance1.gif", "/dance2.gif", "/dance3.gif" };

	constexpr static float notePerfectY = circlesY + 4;
	constexpr static float noteTolerance = 8.0f;
	void updateNotes(float delta);
	void createNote(uint8_t track);
	constexpr static float defaultNotesSpeed = 50.0f;
	float notesSpeed = defaultNotesSpeed;
	void updateTracks(float delta);

	void noteHit(uint8_t track);
	void adjustTempo();
	void adjustScoreBar();

	void hideBars(float deltaTime);
};


#endif //CIRCUITPET_FIRMWARE_GAME5_H
