#ifndef CHATTER_FIRMWARE_LVMODAL_H
#define CHATTER_FIRMWARE_LVMODAL_H

#include "LVObject.h"

class LVScreen;

class LVModal : public LVObject {
public:
	LVModal(LVScreen* parent);
	virtual ~LVModal();

	void start();
	void stop();

	bool isActive() const;

protected:
	lv_group_t* inputGroup;

	virtual void onStart();
	virtual void onStop();

private:
	static LVModal* current;
	bool active = false;

	const LVScreen* parentScreen;
	lv_obj_t* container;


};


#endif //CHATTER_FIRMWARE_LVMODAL_H
