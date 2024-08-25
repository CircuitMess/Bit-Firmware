#include "InstructionsElement.h"

InstructionsElement::InstructionsElement(lv_obj_t* parent, Games game) : LVObject(parent){
	switch(game){
		case Games::Artemis:{
			gameUIPath.append("Artemis/");
			break;
		}
		case Games::Blocks:{
			gameUIPath.append("Blocks/");
			break;
		}
		case Games::Pong:{
			gameUIPath.append("Pong/");
			break;
		}
		case Games::Snake:{
			gameUIPath.append("Snake/");
			break;
		}
		case Games::WackyStacky:{
			gameUIPath.append("Stacky/");
			break;
		}
		case Games::MrBee:{
			gameUIPath.append("Bee/");
			break;
		}
		case Games::Bob:{
			gameUIPath.append("Bob/");
			break;
		}
		case Games::Buttons:{
			gameUIPath.append("Buttons/");
			break;
		}
		case Games::Capacitron:{
			gameUIPath.append("Capacitron/");
			break;
		}
		case Games::Hertz:{
			gameUIPath.append("Hertz/");
			break;
		}
		case Games::Marv:{
			gameUIPath.append("Marv/");
			break;
		}
		case Games::Resistron:{
			gameUIPath.append("Resistron/");
			break;
		}
		case Games::Robby:{
			gameUIPath.append("Robby/");
			break;
		}
		case Games::Harald:{
			gameUIPath.append("Harald/");
			break;
		}
		case Games::Frank:{
			gameUIPath.append("Frank/");
			break;
		}
		case Games::Charlie:{
			gameUIPath.append("Charlie/");
			break;
		}
		case Games::Fred:{
			gameUIPath.append("Fred/");
			break;
		}
		case Games::Planck:{
			gameUIPath.append("Planck/");
			break;
		}
		case Games::Dusty:{
			gameUIPath.append("Dusty/");
			break;
		}
		case Games::Sparkly:{
			gameUIPath.append("Sparkly/");
			break;
		}
		case Games::COUNT:
		default:{
			break;
		}
	}

	buildUI();
}

void InstructionsElement::buildUI(){
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto bg = lv_img_create(*this);
	lv_obj_set_size(bg, 128, 128);
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);
	lv_img_set_src(bg, (gameUIPath + "bg.bin").c_str());

	auto title = lv_img_create(*this);
	lv_obj_set_size(title, 128, 40);
	lv_img_set_src(title, (gameUIPath + "title.bin").c_str());

	auto instr = lv_img_create(*this);
	lv_obj_set_size(instr, 128, 88);
	lv_img_set_src(instr, (gameUIPath + "inst.bin").c_str());
}
