#ifndef MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_
#define MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_

#include "renderer/font.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"
#include "sound/sound_assets.hpp"

#include <filesystem>

#include <audeo/audeo.hpp>

namespace fs = std::filesystem;

namespace munchkin {
namespace assets {
namespace loaders {

template<typename T> struct LoadParams;

template<> struct LoadParams<renderer::Texture> { fs::path path; };

template<> struct LoadParams<renderer::Shader> {
    fs::path vert;
    fs::path frag;
};

template<> struct LoadParams<sound::SoundEffect> { fs::path path; };
template<> struct LoadParams<sound::Music> { fs::path path; };

template<> struct LoadParams<renderer::Font> { fs::path path; };

void load(renderer::Texture& texture, LoadParams<renderer::Texture> const& params);
void load(renderer::Shader& shader, LoadParams<renderer::Shader> const& params);
void load(renderer::Font& font, LoadParams<renderer::Font> const& params);
void load(sound::SoundEffect& sound, LoadParams<sound::SoundEffect> const& params);
void load(sound::Music& music, LoadParams<sound::Music> const& params);

void free(renderer::Texture& texture);
void free(renderer::Shader& shader);
inline void free(renderer::Font& font) {} /* This is taken care of by the Font destructor */
inline void free(sound::SoundEffect& sound) {}   /* This is taken care of by the SoundSource destructor */
inline void free(sound::Music& music) {}         /* This is taken care of by the SoundSource destructor */

} // namespace loaders

} // namespace assets
} // namespace munchkin

#endif