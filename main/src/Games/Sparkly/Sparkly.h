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

	File skybox;

	static constexpr DRAM_ATTR glm::vec3 Up = { 0, 0, 1 };

	static constexpr DRAM_ATTR int Field[20][20] = {
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 34, 23, 23, 23, 23, 23, 23, 32, 30, 30 },
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 34, 21, 3, 7, 7, 7, 7, 8, 25, 30, 30 },
			{ 30, 31, 23, 23, 23, 37, 30, 30, 34, 21, 11, 19, 26, 26, 26, 20, 9, 25, 30, 30 },
			{ 30, 24, 6, 7, 15, 22, 37, 30, 38, 20, 16, 22, 37, 30, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 19, 20, 16, 22, 37, 30, 38, 20, 16, 22, 37, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 38, 20, 16, 22, 37, 30, 38, 20, 0, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 38, 20, 16, 22, 37, 30, 24, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 38, 20, 16, 22, 37, 24, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 35, 17, 29, 30, 30, 30, 38, 20, 16, 22, 21, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 38, 20, 2, 7, 14, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 38, 26, 26, 26, 27, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 30, 34, 23, 23, 23, 32, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 34, 21, 3, 7, 8, 25, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 34, 21, 11, 19, 20, 9, 25, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 34, 21, 11, 19, 35, 24, 9, 22, 21, 9, 25, 30, 30 },
			{ 30, 24, 9, 22, 23, 23, 23, 21, 11, 19, 35, 30, 24, 12, 7, 7, 14, 25, 30, 30 },
			{ 30, 24, 12, 7, 7, 7, 7, 13, 19, 35, 30, 30, 36, 26, 26, 26, 26, 27, 30, 30 },
			{ 30, 36, 26, 26, 26, 26, 26, 26, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
	};

	static constexpr glm::vec2 DRAM_ATTR Billboards[] = {
			{ 1.0f, 1.0f },
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
			{ 0.0f, 0.25f },
	};

	const glm::mat4 Proj;

	std::vector<GameObjPtr> billboardGameObjs;

private:
	void sampleGround(Sprite& canvas);
	uint16_t sampleSkybox(int x, int y);
	void movement(float dt);
	void positionBillboards();
};
} // Sparkly

#endif //BIT_FIRMWARE_SPARKLY_H