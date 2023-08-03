#ifndef CIRCUITPET_FIRMWARE_GAME_H
#define CIRCUITPET_FIRMWARE_GAME_H

#include "ResourceManager.h"
#include "GameObject.h"
#include <set>
#include <vector>
#include "GameSystem.h"
#include "Collision/CollisionSystem.h"
#include "Rendering/RenderSystem.h"
#include <Devices/Display.h>
#include <File/File.h>
#include <Util/Threaded.h>
#include <Util/Events.h>

class Game {

	friend GameSystem;

public:
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

private:
	Sprite& base;
	ResourceManager resMan;
	const std::vector<ResDescriptor> resources;

	bool loaded = false;
	bool started = false;
	ThreadedClosure loadTask;

	volatile bool popped = false;

	RenderSystem render;

	std::set<GameObjPtr> objects;

	void loadFunc();
};


#endif //CIRCUITPET_FIRMWARE_GAME_H
