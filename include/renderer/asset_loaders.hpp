#ifndef MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_
#define MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_

#include "font.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <filesystem>

#include <audeo/audeo.hpp>

namespace fs = std::filesystem;

namespace munchkin {
namespace assets {

template<audeo::AudioType T> struct Sound { audeo::SoundSource source; };
using Music = Sound<audeo::AudioType::Music>;
using SoundEffect = Sound<audeo::AudioType::Effect>;

namespace loaders {

template<typename T> struct LoadParams;

template<> struct LoadParams<renderer::Texture> { fs::path path; };

template<> struct LoadParams<renderer::Shader> {
    fs::path vert;
    fs::path frag;
};

template<> struct LoadParams<SoundEffect> { fs::path path; };
template<> struct LoadParams<Music> { fs::path path; };

template<> struct LoadParams<renderer::Font> { fs::path path; };


void load(renderer::Texture& texture, LoadParams<renderer::Texture> const& params);
void load(renderer::Shader& shader, LoadParams<renderer::Shader> const& params);
void load(renderer::Font& font, LoadParams<renderer::Font> const& params);
void load(SoundEffect& sound, LoadParams<SoundEffect> const& params) {
    std::string path_str = params.path.string();
    audeo::SoundSource s = audeo::load_source(path_str, audeo::AudioType::Effect);
    sound.source = s;
}
void load(Music& music, LoadParams<Music> const& params) {
    std::string path_str = params.path.string();
    audeo::SoundSource s = audeo::load_source(path_str, audeo::AudioType::Music);
    music.source = s;
}

void free(renderer::Texture& texture);
void free(renderer::Shader& shader);
void free(renderer::Font& font) {} /* This is taken care of by the Font destructor */
void free(SoundEffect& sound) {}   /* This is taken care of by the SoundSource destructor */
void free(Music& music) {}         /* This is taken care of by the SoundSource destructor */

} // namespace loaders

} // namespace assets
} // namespace munchkin

#endif