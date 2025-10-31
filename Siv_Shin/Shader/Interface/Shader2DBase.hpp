#pragma once
#include <Siv3D.hpp>
#include "IShader2D.hpp"

class Shader2DBase : public IShader2D
{
public:
	Shader2DBase() = default;
	virtual ~Shader2DBase() = default;

	bool isValid() const noexcept { return m_vs && m_ps; }

	void draw(const Texture& tex, const Vec2& pos) const override
	{
		beginTexture(tex);
		tex.draw(pos);
		end();
	}
	void drawAt(const Texture& tex, const Vec2& center) const override
	{
		beginTexture(tex);
		tex.drawAt(center);
		end();
	}
	void drawScaled(const Texture& tex, const Vec2& pos, double scale) const override
	{
		beginTexture(tex);
		tex.scaled(scale).draw(pos);
		end();
	}
	void drawScaledAt(const Texture& tex, const Vec2& center, double scale) const override
	{
		beginTexture(tex);
		tex.scaled(scale).drawAt(center);
		end();
	}

	void draw(const RectF& s) const override { setShapeInfo(s); beginShape(); s.draw(); end(); }
	void draw(const Circle& s) const override { setShapeInfo(s.boundingRect()); beginShape(); s.draw(); end(); }
	void draw(const Triangle& s) const override { setShapeInfo(s.boundingRect()); beginShape(); s.draw(); end(); }
	void draw(const Ellipse& s) const override { setShapeInfo(s.boundingRect()); beginShape(); s.draw(); end(); }
	void draw(const Quad& s) const override { setShapeInfo(s.boundingRect()); beginShape(); s.draw(); end(); }
	void draw(const RoundRect& s) const override { setShapeInfo(s.rect);          beginShape(); s.draw(); end(); }
	void draw(const Polygon& s) const override { setShapeInfo(s.boundingRect()); beginShape(); s.draw(); end(); }
	void draw(const LineString& s, double t) const override
	{
		setShapeInfo(computeAABB(s, t));
		beginShape();
		s.draw(LineStyle::Default, t);
		end();
	}

protected:
	struct DrawMode { float hasTexture; Float3 _pad; };
	static constexpr uint32 kSlot_DrawMode = 3;
	mutable ConstantBuffer<DrawMode> m_cbDrawMode{ {1.0f, {}} };

	struct ShapeInfo { Float4 rect; Float2 view; Float2 _pad; };
	static constexpr uint32 kSlot_ShapeInfo = 4;
	mutable ConstantBuffer<ShapeInfo> m_cbShape;

	Optional<VertexShader> m_vs;
	Optional<PixelShader>  m_ps;

	mutable std::unique_ptr<ScopedCustomShader2D> m_scope;
	mutable Optional<Texture> m_whiteTex;

	RectF computeAABB(const LineString& ls, double thickness = 1.0) const
	{
		if (ls.isEmpty())
		{
			return RectF{ 0,0,0,0 };
		}
		double minX = std::numeric_limits<double>::infinity();
		double minY = std::numeric_limits<double>::infinity();
		double maxX = -std::numeric_limits<double>::infinity();
		double maxY = -std::numeric_limits<double>::infinity();

		for (const auto& p : ls)
		{
			minX = Min(minX, p.x);
			minY = Min(minY, p.y);
			maxX = Max(maxX, p.x);
			maxY = Max(maxY, p.y);
		}

		const double pad = thickness * 0.5;
		minX -= pad; minY -= pad;
		maxX += pad; maxY += pad;

		return RectF{ minX, minY, (maxX - minX), (maxY - minY) };
	}


	void setCommonCBs() const
	{
		Graphics2D::SetPSConstantBuffer(kSlot_DrawMode, m_cbDrawMode);
		Graphics2D::SetPSConstantBuffer(kSlot_ShapeInfo, m_cbShape);
	}
	void setViewOnly() const
	{
		m_cbShape->rect = Float4{ 0,0,0,0 };
		m_cbShape->view = Float2{ (float)Scene::Width(), (float)Scene::Height() };
		Graphics2D::SetPSConstantBuffer(kSlot_ShapeInfo, m_cbShape);
	}
	void setShapeInfo(const RectF& r) const
	{
		m_cbShape->rect = Float4{ (float)r.x, (float)r.y, (float)r.w, (float)r.h };
		m_cbShape->view = Float2{ (float)Scene::Width(), (float)Scene::Height() };
		Graphics2D::SetPSConstantBuffer(kSlot_ShapeInfo, m_cbShape);
	}

	void beginTextureCommon(const Texture& tex) const
	{
		m_cbDrawMode->hasTexture = 1.0f;
		setCommonCBs();
		m_scope.reset(new ScopedCustomShader2D{ *m_vs, *m_ps });
		Graphics2D::SetPSTexture(0, tex);
	}
	void beginShapeCommon() const
	{
		if (!m_whiteTex) { m_whiteTex.emplace(Image{ 1,1,Color{255,255,255,255} }); }
		m_cbDrawMode->hasTexture = 0.0f;
		setCommonCBs();
		m_scope.reset(new ScopedCustomShader2D{ *m_vs, *m_ps });
		Graphics2D::SetPSTexture(0, *m_whiteTex);
	}
	void endCommon() const
	{
		m_scope.reset();
	}

	void beginTexture(const Texture& tex) const override
	{
		updateEffectCBs();
		setViewOnly();
		beginTextureCommon(tex);
	}
	void beginShape() const override
	{
		updateEffectCBs();
		beginShapeCommon();
	}
	void end() const override
	{
		endCommon();
	}

	virtual void updateEffectCBs() const = 0;
};
