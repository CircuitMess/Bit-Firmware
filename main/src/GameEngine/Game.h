#ifndef BIT_FIRMWARE_GAME_H
#define BIT_FIRMWARE_GAME_H

#include "ResourceManager.h"
#include "GameObject.h"
#include <set>
#include <vector>
#include "GameSystem.h"
#include "Collision/CollisionSystem.h"
#include "Rendering/RenderSystem.h"
#include <Devices/Display.h>
#include <FS/File.h>
#include <Util/Threaded.h>
#include <Util/Events.h>
#include "Devices/Input.h"
#include "Services/ChirpSystem.h"
#include <atomic>

class Game {

	friend GameSystem;

public:
	virtual ~Game();

	void load();
	bool isLoaded() const;

	void start();
	void stop();

	void loop(uint micros);

protected:
	Game(Sprite& base, const char* root, std::vector<ResDescriptor> resources);

	virtual void onStart();
	virtual void onStop();
	virtual void onLoad();
	virtual void onLoop(float deltaTime);
	virtual void onRender(Sprite& canvas);

	File getFile(const std::string& path);

	void addObject(const GameObjPtr& obj);
	void removeObject(const GameObjPtr& obj);

	CollisionSystem collision;

	EventQueue inputQueue;
	virtual void handleInput(const Input::Data& data);

	ChirpSystem& audio;

	void exit();

private:
	Sprite& base;
	ResourceManager resMan;
	const std::vector<ResDescriptor> resources;

	std::atomic_bool loaded = false;
	bool started = false;
	ThreadedClosure loadTask;

	volatile bool popped = false;

	RenderSystem render;

	std::set<GameObjPtr> objects;

	void loadFunc();
};


#endif //BIT_FIRMWARE_GAME_H
