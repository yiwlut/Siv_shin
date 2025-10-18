#pragma once
#include <Siv3D.hpp>

class IShader2D
{
public:
	virtual ~IShader2D() = default;

	// Texture path
	virtual void draw(const Texture& tex, const Vec2& pos) const = 0;
	virtual void drawAt(const Texture& tex, const Vec2& center) const = 0;
	virtual void drawScaled(const Texture& tex, const Vec2& pos, double scale) const = 0;
	virtual void drawScaledAt(const Texture& tex, const Vec2& center, double scale) const = 0;

	// Shape path
	virtual void draw(const RectF& shape) const = 0;
	virtual void draw(const Circle& shape) const = 0;
	virtual void draw(const Triangle& shape) const = 0;
	virtual void draw(const Ellipse& shape) const = 0;
	virtual void draw(const Quad& shape) const = 0;
	virtual void draw(const RoundRect& shape) const = 0;
	virtual void draw(const Polygon& shape) const = 0;
	virtual void draw(const LineString& shape, double thickness) const = 0;

protected:
	virtual void beginTexture(const Texture& tex) const = 0;  // u_hasTexture=1
	virtual void beginShape() const = 0;                      // u_hasTexture=0
	virtual void end() const = 0;
};
