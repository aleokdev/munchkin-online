#include "api/card.hpp"
#include "util/pos_vec.hpp"

namespace munchkin {

namespace renderer {

class CardSprite {
public:
	CardSprite(CardPtr);

	void set_target_pos(math::Vec2D target);
	math::Vec2D get_current_pos();
	void instantly_set_pos(math::Vec2D target);

	void draw();

private:
	math::Vec2D target_pos;
	math::Vec2D current_pos;
	const int movement_slowness = 8;
	CardPtr card;
};

}

}