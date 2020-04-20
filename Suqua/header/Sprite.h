#pragma once
#include "Vec2.h"
#include "ImgData.h"
#include <string>
#include "IDrawable.h"

class Sprite : public IDrawable{
public:
	Sprite(const std::string &filePath, Vec2f pos = {0.0, 0.0}, Vec2f origin_ = { 0.0, 0.0 }, Vec2f scale_ = { 1.0, 1.0 }, Vec2f imgOffset_ = { 0.0f, 0.0f }, float angle_ = 0.0f);
	virtual ~Sprite() override;

	Vec2f getImgRes() const;
	Vec2f getPos() const;
	Vec2f getImgOffset() const;
	Vec2f getObjRes() const;
	Vec2f getOrigin() const;
	Vec2f getScale() const;
	float getAngle() const;
	const ImgData & getImgData() const;

	void setPos(Vec2f pos_) override;
	void setImgOffset(Vec2f imgOffset_);
	void setObjRes(Vec2f objRes_);
	void setOrigin(Vec2f origin_);
	void setScale(Vec2f scale_);
	void setAngle(float angle_);

	int getChannels() const;

	unsigned int getTexture() const; 
	unsigned int getRenderBufferId() const;

	virtual void draw() override;
	virtual IDrawable* clone() override;
protected:
	ImgData data;
	int channels;
	unsigned int texture;
	unsigned int renderBufferId;
};