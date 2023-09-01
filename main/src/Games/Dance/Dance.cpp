#include "Dance.h"
#include "GameEngine/Collision/CircleCC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Rendering/StaticRC.h"

std::map<Input::Button, uint8_t> btnBarMap = { { Input::Button::Left,  0 },
											   { Input::Button::Up,    1 },
											   { Input::Button::Right, 2 } };

Dance::Dance(Sprite& base) : Game(base, "/Games/Buttons", {
		{ barsIcons[0], {}, true },
		{ barsIcons[1], {}, true },
		{ barsIcons[2], {}, true },
		{ barsIcons[3], {}, true },
		{ circlesIcons[0], {}, true },
		{ circlesIcons[1], {}, true },
		{ circlesIcons[2], {}, true },
		{ circlesIconsPressed[0], {}, true },
		{ circlesIconsPressed[1], {}, true },
		{ circlesIconsPressed[2], {}, true },
		{ notesIcon, {}, true },
		{ "/bg.raw", {}, true },
		{ danceGIFs[0].path, {}, false },
		{ danceGIFs[1].path, {}, false },
		{ danceGIFs[2].path, {}, false },
		{ "/lose.gif", {}, false },
		{ "/idle.gif", {}, false },
		{ "/win.gif", {}, false },
		{ "/fail.gif", {}, false },
		RES_HEART }),
							 bottomWall(nullptr, std::make_unique<RectCC>(glm::vec2{ 128, 20 })),
							 scoreBar(std::make_shared<GameObject>(std::make_unique<SpriteRC>(PixelDim{ 118, 5 }), nullptr)){

	bottomWall.setPos({ 0, 138 });
	addObject(std::make_shared<GameObject>(bottomWall));

	scoreBar->setPos({ 5, 3 });
	scoreBarSprite = std::static_pointer_cast<SpriteRC>(scoreBar->getRenderComponent())->getSprite();
	scoreBarSprite->clear(TFT_TRANSPARENT);
	scoreBarSprite->drawRoundRect(0, 0, scoreBarSprite->width(), scoreBarSprite->height(), 1, TFT_WHITE);
	addObject(scoreBar);

	robot = std::make_shared<RoboCtrl::Buttons>();
	setRobot(robot);
}

void Dance::onStart(){
	playerRC->start();
}

void Dance::onStop(){
	playerRC->stop();
	handleInput({ Input::Button::Left, Input::Data::Release });
	handleInput({ Input::Button::Up, Input::Data::Release });
	handleInput({ Input::Button::Right, Input::Data::Release });
}

void Dance::onLoad(){
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }), nullptr);
	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 128 - 31 - 2, 10 });
	addObject(hearts->getGO());

	for(uint8_t i = 0; i < 3; ++i){
		bars[i] = std::make_shared<GameObject>(std::make_unique<StaticRC>(getFile(barsIcons[i]), PixelDim{ 8, 105 }), nullptr);
		bars[i]->setPos({ barsX[i], barsY });
		addObject(bars[i]);

		circles[i] = std::make_shared<GameObject>(std::make_unique<StaticRC>(getFile(circlesIcons[i]), PixelDim{ 12, 12 }),
												  std::make_unique<CircleCC>(9, glm::vec2{ 9, 9 }));
		circles[i]->getRenderComponent()->setLayer(1);
		circles[i]->setPos({ circlesX[i], circlesY });
		addObject(circles[i]);
	}

	player = std::make_shared<GameObject>(std::make_unique<AnimRC>(getFile("/idle.gif")), nullptr);
	player->setPos(PlayerPos + idleGIF.offset);
	playerRC = std::static_pointer_cast<AnimRC>(player->getRenderComponent());
	addObject(player);

	streakObj = std::make_shared<GameObject>(std::make_unique<SpriteRC>(PixelDim{ StreakCircleRadius * 2 + 1, StreakCircleRadius * 2 + 1 }), nullptr);
	streakSprite = std::static_pointer_cast<SpriteRC>(streakObj->getRenderComponent());
	addObject(streakObj);
	streakObj->setPos(50, 16);
	drawStreakSprite(streakSprite->getSprite(), streakLevel);
}

void Dance::onLoop(float deltaTime){
	switch(state){
		case Running:
			beatTimer += deltaTime;
			if(beatTimer >= beatInterval){
				beatTimer = 0;
				uint8_t randNotes = rand() % 8;
				for(int i = 0; i < 3; ++i){
					if(randNotes & (1 << i)){
						createNote(i);
					}
				}
			}

			updateNotes(deltaTime);

			updateTracks(deltaTime);

			updateStreak(deltaTime);
			break;

		case DoneAnim:
			if(fail[0] || fail[1] || fail[2]){
				updateTracks(deltaTime);
			}else{
				hideBars(deltaTime);
				updateNotes(deltaTime);
			}

			break;

		case DonePause:
			hideBars(deltaTime);
			updateNotes(deltaTime);

			gameDoneTimer += deltaTime;
			if(gameDoneTimer >= gameDonePause){
				exit();
			}
			break;

		default:
			break;
	}

}


void Dance::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Press){

		if(state != Running) return;

		if(!btnBarMap.count(data.btn)) return;

		uint8_t circleIndex = btnBarMap[data.btn];

		noteHit(circleIndex);

		if(circlesPressed[circleIndex]) return;

		circlesPressed[circleIndex] = true;
		std::static_pointer_cast<StaticRC>(circles[circleIndex]->getRenderComponent())->setFile(getFile(circlesIconsPressed[circleIndex]));
	}else if(data.action == Input::Data::Release){
		if(!btnBarMap.count(data.btn)) return;

		uint8_t circleIndex = btnBarMap[data.btn];
		if(!circlesPressed[circleIndex]) return;

		circlesPressed[circleIndex] = false;
		std::static_pointer_cast<StaticRC>(circles[circleIndex]->getRenderComponent())->setFile(getFile(circlesIcons[circleIndex]));
	}
}

void Dance::updateNotes(float delta){
	for(uint8_t i = 0; i < 3; ++i){
		for(auto& note : notes[i]){
			note->setPos({ note->getPos().x, note->getPos().y + delta * notesSpeed });
		}
	}
}

void Dance::createNote(uint8_t track){
	auto note = std::make_shared<GameObject>(std::make_unique<StaticRC>(getFile(notesIcon), PixelDim{ 4, 4 }),
											 std::make_unique<RectCC>(glm::vec2{ 4, 4 }));

	note->getRenderComponent()->setLayer(2);
	note->setPos({ barsX[track] + 2, -9 });

	collision.addPair(*note, bottomWall, [this, track](){
		removeObject(notes[track].front());
		notes[track].pop_front();
		resetStreak();
	});

	notes[track].push_back(note);
	addObject(note);
}

void Dance::noteHit(uint8_t track){
	float diff;
	if(notes[track].empty()){
		diff = noteTolerance + 1; //incorrect note hit even when no notes are present on track
	}else{
		diff = abs(notePerfectY - notes[track].front()->getPos().y);
	}


	if(diff <= noteTolerance){
		incrementStreak();
		audio.play(StreakSounds[streakLevel]);
		robot->play(StreakSounds[streakLevel]);
//		RGB.blink(Pixel::Green);

		score += notePoints + (int) (diff * perfectBonus / noteTolerance); //TODO - apply streak multiplier since 3x instantly wins the game

		adjustTempo();
		adjustScoreBar();

		removeObject(notes[track].front());
		notes[track].pop_front();
		if(score >= goal){
			gameDone(true);
			return;
		}

		if(danceAnimSet.empty()){
			for(int i = 0; i < sizeof(danceGIFs) / sizeof(danceGIFs[0]); ++i){
				danceAnimSet.insert(i);
			}
		}
		uint8_t i = rand() % (danceAnimSet.size());
		auto it = danceAnimSet.begin();
		std::advance(it, i);
		i = *it;
		danceAnimSet.erase(it);

		playerRC->setAnim(getFile(danceGIFs[i].path));
		player->setPos(PlayerPos + danceGIFs[i].offset);

		playerRC->setLoopMode(GIF::LoopMode::Infinite);
		playerRC->setLoopDoneCallback([this](uint32_t){
			playerRC->setAnim((getFile("/idle.gif")));
			player->setPos(PlayerPos + idleGIF.offset);
		});

	}else{
		resetStreak();
		Sound bad = { { 300, 300, 50 },
					  { 0,   0,   50 },
					  { 300, 300, 50 } };
		robot->play(bad);
		audio.play(bad);
//		RGB.blinkTwice(Pixel::Red);

		life--;
		hearts->setLives(life);

		fail[track] = true;
		failTime[track] = 0;

		if(life <= 0){
			gameDone(false);
			return;
		}else{
			danceAnimSet.clear();
			playerRC->setAnim(getFile("/fail.gif"));
			player->setPos(PlayerPos + failGIF.offset);


			playerRC->setLoopMode(GIF::LoopMode::Infinite);
			playerRC->setLoopDoneCallback([this](uint32_t){
				playerRC->setAnim((getFile("/idle.gif")));
				player->setPos(PlayerPos + idleGIF.offset);
			});

		}
	}
}

void Dance::adjustTempo(){
	float factor = 1.0f + 0.75f * ((float) (std::min(score, goal)) / (float) goal);
	beatInterval = defaultBeatInterval / factor;
	notesSpeed = defaultNotesSpeed * factor;
}

void Dance::gameDone(bool success){
	if(success){
		Sound s = { { 300, 300,  50 },
					{ 0,   0,    20 },
					{ 450, 450,  50 },
					{ 0,   0,    20 },
					{ 300, 300,  50 },
					{ 0,   0,    20 },
					{ 600, 600,  50 },
					{ 0,   0,    20 },
					{ 300, 300,  50 },
					{ 0,   0,    20 },
					{ 600, 600,  50 },
					{ 0,   0,    20 },
					{ 1000, 1000, 300 } };
		robot->play(s);
		audio.play(s);
		playerRC->setAnim(getFile("/win.gif"));
		player->setPos(PlayerPos + winGIF.offset);

	}else{
		Sound s = { { 600, 600,  50 },
					{ 0,   0,    20 },
					{ 450, 450,  50 },
					{ 0,   0,    20 },
					{ 300, 300,  50 },
					{ 0,   0,    20 },
					{ 450, 450,  50 },
					{ 0,   0,    20 },
					{ 300, 300,  50 },
					{ 0,   0,    20 },
					{ 200, 200,  50 },
					{ 0,   0,    20 },
					{ 100, 100, 300 } };
		robot->play(s);
		audio.play(s);
		playerRC->setAnim(getFile("/lose.gif"));
		player->setPos(PlayerPos + loseGIF.offset);
	}

	state = DoneAnim;

	playerRC->setLoopMode(GIF::LoopMode::Single);
	playerRC->setLoopDoneCallback([this](uint32_t){
		state = DonePause;
		gameDoneTimer = 0;
		playerRC->stop();
	});
}

void Dance::adjustScoreBar(){
	int infill = (int) ((float) (scoreBarSprite->width() - 2) * ((float) (std::min(score, goal)) / (float) goal));

	scoreBarSprite->fillRect(scoreBarSprite->width() - 1 - infill, 1, infill, scoreBarSprite->height() - 2, C_RGB(230, 72, 46));
}

void Dance::hideBars(float deltaTime){
	for(int i = 0; i < 3; i++){
		auto pos = bars[i]->getPos();
		pos.y -= 80.0f * deltaTime;
		bars[i]->setPos(pos);

		pos = circles[i]->getPos();
		pos.y -= 80.0f * deltaTime;
		circles[i]->setPos(pos);
	}
}

void Dance::updateTracks(float delta){
	for(uint8_t i = 0; i < 3; i++){
		if(fail[i]){
			failTime[i] += delta;

			auto rc = std::static_pointer_cast<StaticRC>(bars[i]->getRenderComponent());
			if((int) (failTime[i] / failBlinkDuration) % 2 == 0){
				rc->setFile(getFile(barsIcons[3]));
			}else{
				rc->setFile(getFile(barsIcons[i]));
			}

			if(failTime[i] >= failDuration){
				failTime[i] = 0;
				fail[i] = false;
				rc->setFile(getFile(barsIcons[i]));
			}
		}
	}
}

void Dance::updateStreak(float delta){
	if(!streakAnimActive) return;
	streakAnimT += delta;
	const auto scale = 1.0 + 0.5 * abs(sin((2 * M_PI / StreakAnimPeriod) * streakAnimT));
	streakSprite->setScale(scale);

	if(streakAnimT >= StreakAnimPeriod){
		streakAnimT = 0;
		streakAnimActive = false;
	}
}

void Dance::drawStreakSprite(std::shared_ptr<Sprite> sprite, uint8_t level){
	//TODO - promjeniti vizual ovoga u nešto ljepše
	sprite->clear(TFT_TRANSPARENT);
	sprite->fillCircle(StreakCircleRadius, StreakCircleRadius, StreakCircleRadius, TFT_RED);
	sprite->drawCircle(StreakCircleRadius, StreakCircleRadius, StreakCircleRadius, TFT_WHITE);
	sprite->setTextDatum(lgfx::datum_t::middle_center);
	sprite->setTextColor(TFT_WHITE);
	std::string s = std::to_string(level + 1) + "x";
	sprite->drawString(s.c_str(), StreakCircleRadius, StreakCircleRadius);
}

void Dance::incrementStreak(){
	notesStreak = std::min(notesStreak + 1, (int) MaxStreakLevel * NotesPerStreakLevel);
	if(notesStreak / NotesPerStreakLevel > streakLevel && streakLevel < (MaxStreakLevel - 1)){
		streakLevel = notesStreak / NotesPerStreakLevel;
		drawStreakSprite(streakSprite->getSprite(), streakLevel);
		streakAnimActive = true;
	}

}

void Dance::resetStreak(){
	notesStreak = 0;
	if(streakLevel == 0) return;
	streakLevel = 0;
	streakAnimActive = true;
	drawStreakSprite(streakSprite->getSprite(), streakLevel);
}

