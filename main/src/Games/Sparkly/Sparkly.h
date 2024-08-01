#ifndef BIT_FIRMWARE_SPARKLY_H
#define BIT_FIRMWARE_SPARKLY_H

#include "GameEngine/Game.h"
#include "Transform.h"

namespace Sparkly {
class Sparkly : public Game {
public:
	explicit Sparkly(Sprite& canvas);

	inline virtual uint32_t getXP() const override {
		return 0;
	}

	// TODO need a UI element for the time and UI element for the place in the race
	// TODO how are we going to make the opponents? how are they going to drive?

protected:
	virtual void onLoad() override;
	virtual void onLoop(float deltaTime) override;
	virtual void preRender(Sprite& canvas) override;
	virtual void handleInput(const Input::Data& data);

private:;
	glm::mat4 vpMat; // view-projection matrix
	glm::mat4 vpInv; // inverted view-projection matrix
	Transform cameraTransform = Transform(glm::vec3{ 0.0f, -0.5f, 2.2f });

	float spd = 0; // Forward/backward
	float spdUD = 0; // Up/down
	float spdZ = 0; // Left/right

	static constexpr DRAM_ATTR glm::vec3 Up = { 0, 0, 1 };
	/**
	 * Spritesheet is 96x96 and contains 3x3 sprites, each 32x32 pixels
	 */
	static constexpr DRAM_ATTR int Field[4][4] = {
			{ 30, 31, 31, 32 },
			{ 33, 34, 34, 35 },
			{ 33, 34, 35, 35 },
			{ 36, 36, 37, 38 }
	};

	static constexpr glm::vec2 DRAM_ATTR Billboards[] = {
			{ 1.0f, 1.0f },/*
			{ 1.0f, -0.75f },
			{ 1.0f, -0.5f },
			{ 1.0f, -0.25f },
			{ 1.0f, -0.0f },
			{ 1.0f, 1.0f },
			{ 1.0f, 0.75f },
			{ 1.0f, 0.5f },
			{ 1.0f, 0.25f },

			{ 0.0f, -1.0f },
			{ 0.0f, -0.75f },
			{ 0.0f, -0.5f },
			{ 0.0f, -0.25f },
			{ 0.0f, -0.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.75f },
			{ 0.0f, 0.5f },
			{ 0.0f, 0.25f },*/
	};

	const glm::mat4 Proj;

	std::vector<GameObjPtr> billboardGameObjs;

private:
	void sampleGround(Sprite& canvas);
	void movement(float dt);
	void positionBillboards();
};
} // Sparkly

#endif //BIT_FIRMWARE_SPARKLY_H