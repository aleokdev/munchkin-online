#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include <sol/sol.hpp>

namespace munchkin {

class State {
public:

private:
    sol::state lua_state;
};

}

#endif