#pragma once
#include <imgui.h>
#include <cmath>

namespace pos_vec
{
	struct Vec2D;

	struct Vec2D
	{
		float x;
		float y;

		#define DEFINE_OPERATOR(op)				\
		Vec2D& operator##op##=(const Vec2D& rhs)\
		{\
			this->x op##= rhs.x;\
			this->y op##= rhs.y;\
			return *this;\
		}\
		friend Vec2D operator##op##(Vec2D lhs, const Vec2D& rhs)\
		{\
			lhs op##= rhs;\
			return lhs;\
		}

		DEFINE_OPERATOR(+);
		DEFINE_OPERATOR(-);
		DEFINE_OPERATOR(*);
		DEFINE_OPERATOR(/);

	#undef DEFINE_OPERATOR

		friend Vec2D operator*(Vec2D lhs, const float& rhs)
		{
			lhs.x *= rhs;
			lhs.y *= rhs;
			return lhs;
		}

		friend Vec2D operator/(Vec2D lhs, const float& rhs)
		{
			lhs.x /= rhs;
			lhs.y /= rhs;
			return lhs;
		}

		operator ImVec2() const
		{
			return ImVec2{ (float)x, (float)y };
		}

		static Vec2D from_ImVec(ImVec2 v)
		{
			Vec2D ret;
			ret.x = v.x;
			ret.y = v.y;
			return ret;
		}

		float distance_to(const Vec2D& other) const
		{
			return std::sqrt(std::pow(x - other.x, 2.0f) + std::pow(y - other.y, 2.0f));
		}

		float get_length() const
		{
			return std::sqrt(std::pow(x, 2.0f) + std::pow(y, 2.0f));
		}

		Vec2D invert() const
		{
			Vec2D ret;
			ret.x = y;
			ret.y = x;
			return ret;
		}
	};

	struct Rect2D
	{
		Vec2D pos;
		Vec2D size;

		bool contains(const Vec2D& p) const
		{
			return (
				p.x >= pos.x &&
				p.y >= pos.y &&
				p.x <= (pos.x + size.x) &&
				p.y <= (pos.y + size.y)
				);			
		}

		Vec2D get_center() const
		{
			return pos + (size / 2);
		}
	};

	namespace vectors
	{
		inline static constexpr Vec2D up = Vec2D{ 0, -1 };
		inline static constexpr Vec2D down = Vec2D{ 0, 1 };
		inline static constexpr Vec2D y_axis = Vec2D{ 0, 1 };
		inline static constexpr Vec2D x_axis = Vec2D{ 1, 0 };
		inline static constexpr Vec2D left = Vec2D{ 0, -1 };
		inline static constexpr Vec2D right = Vec2D{ 0, 1 };
		inline static constexpr Vec2D zero = Vec2D{ 0, 0 };
	}
}