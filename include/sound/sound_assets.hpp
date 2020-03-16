#ifndef MUNCHKIN_ONLINE_SOUND_ASSETS_HPP
#define MUNCHKIN_ONLINE_SOUND_ASSETS_HPP

#include <audeo/audeo.hpp>
#include <string>

namespace munchkin::sound {

struct Music {
    audeo::SoundSource source;
    std::string title;
    std::string artist;
};

struct SoundEffect {
    audeo::SoundSource source;
};

} // namespace munchkin::sound

#endif // MUNCHKIN_ONLINE_SOUND_ASSETS_HPP
