#ifndef BIT_FIRMWARE_FILEPATHS_HPP
#define BIT_FIRMWARE_FILEPATHS_HPP

namespace Filepath {
	// TODO need to fill this out after adding and moving things around in the spiffs
	namespace Theme1 {
		inline static constexpr const char* Background = "";
		inline static constexpr const char* Popup = "";
		inline static constexpr const char* Header = "";
		inline static constexpr const char* BarLong = "";
		inline static constexpr const char* BarShort = "";
	}

	namespace Theme2 {
		inline static constexpr const char* Background = "";
		inline static constexpr const char* Popup = "";
		inline static constexpr const char* Header = "";
		inline static constexpr const char* BarLong = "";
		inline static constexpr const char* BarShort = "";
	}

	namespace Theme3 {
		inline static constexpr const char* Background = "";
		inline static constexpr const char* Popup = "";
		inline static constexpr const char* Header = "";
		inline static constexpr const char* BarLong = "";
		inline static constexpr const char* BarShort = "";
	}

	namespace Theme4 {
		inline static constexpr const char* Background = "";
		inline static constexpr const char* Popup = "";
		inline static constexpr const char* Header = "";
		inline static constexpr const char* BarLong = "";
		inline static constexpr const char* BarShort = "";
	}

	namespace Battery {
		inline static constexpr const char* EmptyBig = "S:/Batt/EmptyBig.bin";
		inline static constexpr const char* Low = "S:/Batt/Low.bin";
		inline static constexpr const char* Mid = "S:/Batt/Mid.bin";
		inline static constexpr const char* Full = "S:/Batt/Full.bin";
	}

	namespace Award {

	}

	namespace Profile {

	}

	namespace Achievement {

	}

	namespace Asset {
		namespace Common {
			inline static constexpr const char* Goblet = "S:/Games/Common/goblet.raw";
			inline static constexpr const char* Heart = "S:/Games/Common/heart.raw";
		}

		namespace Pong {
			inline static constexpr const char* Background = "S:/Games/Pong/bg.raw";
		}

		namespace Snake {
			inline static constexpr const char* Apple = "S:/Games/Snake/Apple.raw";
			inline static constexpr const char* Banana = "S:/Games/Snake/Banana.raw";
			inline static constexpr const char* Background = "S:/Games/Snake/bg.raw";
			inline static constexpr const char* Blueberry = "S:/Games/Snake/Blueberry.raw";
			inline static constexpr const char* Cherry = "S:/Games/Snake/Cherry.raw";
			inline static constexpr const char* Grapes = "S:/Games/Snake/Grapes.raw";
			inline static constexpr const char* Icecream = "S:/Games/Snake/Icecream.raw";
			inline static constexpr const char* Lemon = "S:/Games/Snake/Lemon.raw";
			inline static constexpr const char* Plum = "S:/Games/Snake/Plum.raw";
			inline static constexpr const char* Sausage = "S:/Games/Snake/Sausage.raw";
		}

		namespace Robby {
			inline static constexpr const char* Background = "S:/Games/Robby/bg.raw";
			inline static constexpr const char* Bullet = "S:/Games/Robby/bullet.gif";
			inline static constexpr const char* Dead = "S:/Games/Robby/dead.gif";
			inline static constexpr const char* L1 = "S:/Games/Robby/L1.gif";
			inline static constexpr const char* L2 = "S:/Games/Robby/L2.gif";
			inline static constexpr const char* M1 = "S:/Games/Robby/M1.gif";
			inline static constexpr const char* M2 = "S:/Games/Robby/M2.gif";
			inline static constexpr const char* S1 = "S:/Games/Robby/S1.gif";
			inline static constexpr const char* S2 = "S:/Games/Robby/S2.gif";
		}

		namespace Resistron {
			inline static constexpr const char* Background = "S:/Games/Resistron/bg.raw";
			inline static constexpr const char* Bullet = "S:/Games/Resistron/bullet.raw";
			inline static constexpr const char* Dead = "S:/Games/Resistron/dead.gif";
			inline static constexpr const char* Hit = "S:/Games/Resistron/hit.gif";
			inline static constexpr const char* Invader1 = "S:/Games/Resistron/invader1.raw";
			inline static constexpr const char* Invader2 = "S:/Games/Resistron/invader2.raw";
			inline static constexpr const char* Invader3 = "S:/Games/Resistron/invader3.raw";
			inline static constexpr const char* Invader4 = "S:/Games/Resistron/invader4.raw";
			inline static constexpr const char* InvByllet = "S:/Games/Resistron/invBullet.raw";
		}

		namespace Marv {
			inline static constexpr const char* Banana = "S:/Games/Marv/banana.raw";
			inline static constexpr const char* Background = "S:/Games/Marv/bg.raw";
			inline static constexpr const char* Duck = "S:/Games/Marv/duck.gif";
			inline static constexpr const char* Ducking = "S:/Games/Marv/ducking.gif";
			inline static constexpr const char* Fence = "S:/Games/Marv/fence.raw";
			inline static constexpr const char* Hydrant = "S:/Games/Marv/hydrant.raw";
			inline static constexpr const char* Jump = "S:/Games/Marv/jump.gif";
			inline static constexpr const char* Manhole = "S:/Games/Marv/manhole.raw";
			inline static constexpr const char* Newspaper = "S:/Games/Marv/newspaper.raw";
			inline static constexpr const char* PidgeonFly = "S:/Games/Marv/pidgeonFly.raw";
			inline static constexpr const char* PidgeonWalk = "S:/Games/Marv/pidgeonWalk.raw";
			inline static constexpr const char* Podium = "S:/Games/Marv/podium.raw";
			inline static constexpr const char* Teleport = "S:/Games/Marv/tp.raw";
			inline static constexpr const char* Umbrella = "S:/Games/Marv/umbrella.raw";
			inline static constexpr const char* Unducking = "S:/Games/Marv/unducking.gif";
			inline static constexpr const char* Walk = "S:/Games/Marv/walk.gif";
			inline static constexpr const char* WalkDead = "S:/Games/Marv/walkDead.gif";
			inline static constexpr const char* Weeds = "S:/Games/Marv/weeds.raw";
			inline static constexpr const char* Win = "S:/Games/Marv/win.gif";
		}

		namespace Hertz {
			inline static constexpr const char* Arrow = "S:/Games/Hertz/Arrow.raw";
			inline static constexpr const char* Background = "S:/Games/Hertz/bg.raw";
			inline static constexpr const char* Blink = "S:/Games/Hertz/blink.raw";
			inline static constexpr const char* Idle = "S:/Games/Hertz/idle.raw";
			inline static constexpr const char* Win = "S:/Games/Hertz/win.raw";
		}

		namespace Flappy {
			inline static constexpr const char* Bee = "S:/Games/Flappy/Bee.gif";
			inline static constexpr const char* Background = "S:/Games/Flappy/bg.raw";
			inline static constexpr const char* Down1 = "S:/Games/Flappy/down1.raw";
			inline static constexpr const char* Down2 = "S:/Games/Flappy/down2.raw";
			inline static constexpr const char* Down3 = "S:/Games/Flappy/down3.raw";
			inline static constexpr const char* Down4 = "S:/Games/Flappy/down4.raw";
			inline static constexpr const char* Down5 = "S:/Games/Flappy/down5.raw";
			inline static constexpr const char* Down6 = "S:/Games/Flappy/down6.raw";
			inline static constexpr const char* Up1 = "S:/Games/Flappy/up1.raw";
			inline static constexpr const char* Up2 = "S:/Games/Flappy/up2.raw";
			inline static constexpr const char* Up3 = "S:/Games/Flappy/up3.raw";
			inline static constexpr const char* Up4 = "S:/Games/Flappy/up4.raw";
			inline static constexpr const char* Up5 = "S:/Games/Flappy/up5.gif";
		}

		namespace Bob {
			inline static constexpr const char* Background = "S:/Games/Bob/bg.raw";
			inline static constexpr const char* BarFrame = "S:/Games/Bob/BarFrame.raw";
			inline static constexpr const char* Bomb = "S:/Games/Bob/Bomb.raw";
			inline static constexpr const char* BulbG = "S:/Games/Bob/Bulb-g.raw";
			inline static constexpr const char* BulbY = "S:/Games/Bob/Bulb-y.raw";
			inline static constexpr const char* Dead = "S:/Games/Bob/dead.gif";
			inline static constexpr const char* Dynamite = "S:/Games/Bob/Dynamite.raw";
			inline static constexpr const char* Eat = "S:/Games/Bob/eat.gif";
			inline static constexpr const char* EatBad = "S:/Games/Bob/eatBad.gif";
			inline static constexpr const char* Win = "S:/Games/Bob/win.gif";
		}

		namespace Capacitron {
			inline static constexpr const char* Background1 = "S:/Games/Capacitron/bg1.raw";
			inline static constexpr const char* Background2 = "S:/Games/Capacitron/bg2.raw";
			inline static constexpr const char* Background3 = "S:/Games/Capacitron/bg3.raw";
			inline static constexpr const char* Background4 = "S:/Games/Capacitron/bg4.raw";
			inline static constexpr const char* Dead = "S:/Games/Capacitron/dead.gif";
			inline static constexpr const char* Fireball = "S:/Games/Capacitron/fireball.gif";
			inline static constexpr const char* Heart = "S:/Games/Capacitron/heart.gif";
			inline static constexpr const char* Jump = "S:/Games/Capacitron/jump.gif";
			inline static constexpr const char* JumpGlow = "S:/Games/Capacitron/jumpGlow.gif";
			inline static constexpr const char* JumpPad = "S:/Games/Capacitron/jumpPad.gif";
			inline static constexpr const char* Pad1 = "S:/Games/Capacitron/pad1.raw";
			inline static constexpr const char* Pad2 = "S:/Games/Capacitron/pad2.raw";
			inline static constexpr const char* Pad3 = "S:/Games/Capacitron/pad3.raw";
			inline static constexpr const char* Pad4 = "S:/Games/Capacitron/pad4.raw";
			inline static constexpr const char* PadLeft = "S:/Games/Capacitron/padL.raw";
			inline static constexpr const char* PadRight = "S:/Games/Capacitron/padR.raw";
			inline static constexpr const char* Potion = "S:/Games/Capacitron/potion.raw";
			inline static constexpr const char* WallLeft1 = "S:/Games/Capacitron/wallL1.raw";
			inline static constexpr const char* WallLeft2 = "S:/Games/Capacitron/wallL2.raw";
			inline static constexpr const char* WallLeft3 = "S:/Games/Capacitron/wallL3.raw";
			inline static constexpr const char* WallRight1 = "S:/Games/Capacitron/wallR1.raw";
			inline static constexpr const char* WallRight2 = "S:/Games/Capacitron/wallR2.raw";
			inline static constexpr const char* WallRight3 = "S:/Games/Capacitron/wallR3.raw";
		}

		namespace Buttons {
			inline static constexpr const char* Background = "S:/Games/Buttons/bg.raw";
			inline static constexpr const char* BarB = "S:/Games/Buttons/BarB.raw";
			inline static constexpr const char* BarP = "S:/Games/Buttons/BarP.raw";
			inline static constexpr const char* BarR = "S:/Games/Buttons/BarR.raw";
			inline static constexpr const char* BarY = "S:/Games/Buttons/BarY.raw";
			inline static constexpr const char* CircleB1 = "S:/Games/Buttons/circle-b1.raw";
			inline static constexpr const char* CircleB2 = "S:/Games/Buttons/circle-b2.raw";
			inline static constexpr const char* CircleP1 = "S:/Games/Buttons/circle-p1.raw";
			inline static constexpr const char* CircleP2 = "S:/Games/Buttons/circle-p2.raw";
			inline static constexpr const char* CircleY1 = "S:/Games/Buttons/circle-y1.raw";
			inline static constexpr const char* CircleY2 = "S:/Games/Buttons/circle-y2.raw";
			inline static constexpr const char* Dot = "S:/Games/Buttons/tocka.raw";
			inline static constexpr const char* Dance1 = "S:/Games/Buttons/dance1.gif";
			inline static constexpr const char* Dance2 = "S:/Games/Buttons/dance2.gif";
			inline static constexpr const char* Dance3 = "S:/Games/Buttons/dance3.gif";
			inline static constexpr const char* Fail = "S:/Games/Buttons/fail.gif";
			inline static constexpr const char* Idle = "S:/Games/Buttons/idle.gif";
			inline static constexpr const char* Lose = "S:/Games/Buttons/lose.gif";
			inline static constexpr const char* Win = "S:/Games/Buttons/win.gif";
		}

		namespace Blocks {
			inline static constexpr const char* Cyan = "S:/Games/Blocks/cyan.raw";
			inline static constexpr const char* Gold = "S:/Games/Blocks/gold.raw";
			inline static constexpr const char* Green = "S:/Games/Blocks/green.raw";
			inline static constexpr const char* Placed = "S:/Games/Blocks/placed.raw";
			inline static constexpr const char* Purple = "S:/Games/Blocks/purple.raw";
			inline static constexpr const char* Red = "S:/Games/Blocks/red.raw";
			inline static constexpr const char* Yellow = "S:/Games/Blocks/yellow.raw";
		}

		namespace Artemis {
			inline static constexpr const char* Aim = "S:/Games/Arte/aim.raw";
			inline static constexpr const char* Background = "S:/Games/Arte/bg.raw";
			inline static constexpr const char* BackgroundBottom = "S:/Games/Arte/bg_bot.raw";
			inline static constexpr const char* CurtLeft = "S:/Games/Arte/curt_l.raw";
			inline static constexpr const char* CurtRight = "S:/Games/Arte/curt_r.raw";
			inline static constexpr const char* Debree = "S:/Games/Arte/debree.raw";
			inline static constexpr const char* HitStick1 = "S:/Games/Arte/hit_stick1.gif";
			inline static constexpr const char* HitStick2 = "S:/Games/Arte/hit_stick2.gif";
			inline static constexpr const char* HitStick3 = "S:/Games/Arte/hit_stick3.gif";
			inline static constexpr const char* HitStick4 = "S:/Games/Arte/hit_stick4.gif";
			inline static constexpr const char* HitStick5 = "S:/Games/Arte/hit_stick5.gif";
			inline static constexpr const char* Stick = "S:/Games/Arte/stick.raw";
			inline static constexpr const char* Stick1 = "S:/Games/Arte/stick1.raw";
			inline static constexpr const char* Stick2 = "S:/Games/Arte/stick2.raw";
			inline static constexpr const char* Stick3 = "S:/Games/Arte/stick3.raw";
			inline static constexpr const char* Stick4 = "S:/Games/Arte/stick4.raw";
			inline static constexpr const char* Stick5 = "S:/Games/Arte/stick5.raw";
			inline static constexpr const char* WaveBack = "S:/Games/Arte/wave_back.raw";
			inline static constexpr const char* WaveFront = "S:/Games/Arte/wave_front.raw";
			inline static constexpr const char* Win1 = "S:/Games/Arte/win1.raw";
			inline static constexpr const char* Win2 = "S:/Games/Arte/win2.raw";
			inline static constexpr const char* Win3 = "S:/Games/Arte/win3.raw";
			inline static constexpr const char* Windows = "S:/Games/Arte/windows.raw";
		}

		namespace WackyStacky {

		}

		namespace Sparkly {

		}

		namespace Planck {

		}

		namespace Harald {

		}

		namespace Fred {

		}

		namespace Frank {

		}

		namespace Dusty {

		}
	}
}

#endif //BIT_FIRMWARE_FILEPATHS_HPP