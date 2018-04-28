#pragma once

class Sprite
{
public:
	Sprite(class Texture* texture, float u = 0, float v = 0, float uvWidth = 1, float uvHeight = 1);

	~Sprite();

	//Accessors
	class Texture* texture() const;
	void texture(class Texture* val);
	float u() const;
	void u(float val);
	float v() const;
	void v(float val);
	float u2() const;
	void u2(float val);
	float v2() const;
	void v2(float val);

private:
	class Texture* _texture;
	float _u, _v, _u2, _v2;
};
