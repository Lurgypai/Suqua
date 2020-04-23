#pragma once
#include "Vec2.h"
#include "ImgData.h"
#include <string>
#include "IDrawable.h"

class Sprite : public IDrawable{
public:
	Sprite(const std::string &texture_tag);
	virtual ~Sprite() override;

	Vec2f getImgRes() const;
	Vec2f getPos() const;
	Vec2f getImgOffset() const;
	Vec2f getObjRes() const;
	Vec2f getOrigin() const;
	Vec2f getScale() const;
	float getAngle() const;
	const ImgData & getImgData() const;

	void setImgRes(Vec2f res_);
	void setPos(Vec2f pos_) override;
	void setImgOffset(Vec2f imgOffset_);
	void setObjRes(Vec2f objRes_);
	void setOrigin(Vec2f origin_);
	void setScale(Vec2f scale_);
	void setAngle(float angle_);

	int getChannels() const;

	virtual void draw() override;
	virtual IDrawable* clone() override;
protected:
	std::string texture_tag;
	ImgData data;
	int channels;
};