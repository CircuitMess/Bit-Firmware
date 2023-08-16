#include "TextRC.h"

TextRC::TextRC(std::string text, TextStyle style) : text(text), style(style){
}

void TextRC::push(Sprite& parent, PixelDim pos, float rot) const{
	parent.setFont(style.font);
	parent.setTextDatum(style.datum);
	parent.setTextColor(style.color);
	parent.setTextSize(style.scale);

	parent.drawString(text.c_str(), pos.x, pos.y);
}

void TextRC::setText(std::string text){
	this->text = text;
}

void TextRC::setStyle(TextStyle style){
	this->style = style;
}
