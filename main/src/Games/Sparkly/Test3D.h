#ifndef BIT_FIRMWARE_TEST3D_H
#define BIT_FIRMWARE_TEST3D_H

#include "Devices/Display.h"
#include <vec3.hpp>
#include <gtc/quaternion.hpp>
#include "Util/Events.h"
#include "GameEngine/Game.h"

#define Count(x) (sizeof(x)/sizeof(x[0]))

class Test3D : public Game {
public:
	Test3D(Sprite& base);

	inline virtual uint32_t getXP() const override {
		return 0;
	}

protected:
	virtual void onLoad() override;
	virtual void preRender(Sprite& canvas) override;
	virtual void onLoop(float dt) override;

private:
	Sprite& canvas;

	EventQueue evts;

	static constexpr DRAM_ATTR glm::vec3 Up = { 0, 0, 1 };
	const glm::mat4 Proj;

	/**
	 * Spritesheet is 96x96 and contains 3x3 sprites, each 32x32 pixels
	 */
	static constexpr DRAM_ATTR int Field[4][4] = {
			{ 30, 31, 31, 32 },
			{ 33, 34, 34, 35 },
			{ 33, 34, 35, 35 },
			{ 36, 36, 37, 38 }
	};

	struct Asset {
		const char* path;
		glm::vec<2, int> dim;
	};
	static constexpr DRAM_ATTR Asset Assets[] = {
			{ "/Tilemap.raw", { 224, 192 } },
			{ "/Light-r.raw", { 27, 60 } },
			{ "/Bush1.raw", { 32, 28 } },
	};

	struct BB {
		int index;
		glm::vec2 pos;
		float scale;
	};
	static constexpr BB DRAM_ATTR Boards[] = {
			{ 2, { 1.0f, -2.0f }, 1 },
			{ 2, { 1.0f, -1.75f }, 1 },
			{ 2, { 1.0f, -1.5f }, 1 },
			{ 2, { 1.0f, -1.25f }, 1 },
			{ 2, { 1.0f, -1.0f }, 1 },
			{ 2, { 1.0f, -0.75f }, 1 },
			{ 2, { 1.0f, -0.5f }, 1 },
			{ 2, { 1.0f, -0.25f }, 1 },
			{ 2, { 1.0f, -0.0f }, 1 },
			{ 2, { 1.0f, 2.0f }, 1 },
			{ 2, { 1.0f, 1.75f }, 1 },
			{ 2, { 1.0f, 1.5f }, 1 },
			{ 2, { 1.0f, 1.25f }, 1 },
			{ 2, { 1.0f, 1.0f }, 1 },
			{ 2, { 1.0f, 0.75f }, 1 },
			{ 2, { 1.0f, 0.5f }, 1 },
			{ 2, { 1.0f, 0.25f }, 1 },

			{ 2, { 0.0f, -2.0f }, 1 },
			{ 2, { 0.0f, -1.75f }, 1 },
			{ 2, { 0.0f, -1.5f }, 1 },
			{ 2, { 0.0f, -1.25f }, 1 },
			{ 2, { 0.0f, -1.0f }, 1 },
			{ 2, { 0.0f, -0.75f }, 1 },
			{ 2, { 0.0f, -0.5f }, 1 },
			{ 2, { 0.0f, -0.25f }, 1 },
			{ 2, { 0.0f, -0.0f }, 1 },
			{ 2, { 0.0f, 2.0f }, 1 },
			{ 2, { 0.0f, 1.75f }, 1 },
			{ 2, { 0.0f, 1.5f }, 1 },
			{ 2, { 0.0f, 1.25f }, 1 },
			{ 2, { 0.0f, 1.0f }, 1 },
			{ 2, { 0.0f, 0.75f }, 1 },
			{ 2, { 0.0f, 0.5f }, 1 },
			{ 2, { 0.0f, 0.25f }, 1 },
	};

	glm::vec3 camPos = { 0, -0.5f, 2.2f };
	float rotUD = -80;
	float rotZ = 0;

	float spd = 0; // Forward/backward
	float spdUD = 0; // Up/down
	float spdZ = 0; // Left/right

	// Per frame
	glm::vec3 forward;
	glm::mat4 vpMat; // view-projection matrix
	glm::mat4 vpInv; // inverted view-projection matrix

	std::vector<GameObjPtr> billboardGameObjs;

	void movement(float dt);
	void calcView();
	void renderGround();
	void renderBillboards();
	uint16_t sampleGround(float planeX, float planeY);

};


#endif //BIT_FIRMWARE_TEST3D_H
