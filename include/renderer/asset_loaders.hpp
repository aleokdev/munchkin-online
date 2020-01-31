#ifndef MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_
#define MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_

#include "texture.hpp"
#include "shader.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace munchkin::assets {

namespace loaders {

template<typename T>
struct LoadParams;

template<>
struct LoadParams<renderer::Texture> {
    fs::path path;
};

template<>
struct LoadParams<renderer::Shader> {
    fs::path vert;
    fs::path frag;
};

void load(renderer::Texture& texture, LoadParams<renderer::Texture> const& params);
void load(renderer::Shader& shader, LoadParams<renderer::Shader> const& params);

void free(renderer::Texture& texture);
void free(renderer::Shader& shader);

}

}

#endif