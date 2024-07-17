#ifndef BIT_FIRMWARE_GRAYSCALEIMAGEELEMENT_H
#define BIT_FIRMWARE_GRAYSCALEIMAGEELEMENT_H

#include <string>
#include <vector>
#include "LV_Interface/LVObject.h"

class GrayscaleImageElement : public LVObject {
public:
	GrayscaleImageElement(lv_obj_t* parent, const std::string& path, bool grayscale);
	virtual ~GrayscaleImageElement() override = default;

	bool isGrayscale() const;
	void setGrayscale(bool value);

private:
	lv_obj_t* canvas;
	std::vector<uint8_t> canvasData;
	std::string path;
	bool grayscale;
};

#endif //BIT_FIRMWARE_GRAYSCALEIMAGEELEMENT_H
