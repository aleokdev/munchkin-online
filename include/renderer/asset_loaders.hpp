#ifndef MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_
#define MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_

#include "font.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <filesystem>

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

template<> struct LoadParams<renderer::Font> { fs::path path; };

void load(renderer::Texture& texture, LoadParams<renderer::Texture> const& params);
void load(renderer::Shader& shader, LoadParams<renderer::Shader> const& params);
void load(renderer::Font& font, LoadParams<renderer::Font> const& params);

void free(renderer::Texture& texture);
void free(renderer::Shader& shader);
void free(renderer::Font& font);

} // namespace loaders

} // namespace assets
} // namespace munchkin

#endif