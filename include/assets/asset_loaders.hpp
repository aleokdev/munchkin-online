#ifndef MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_
#define MUNCHKIN_ONLINE_ASSET_LOADERS_HPP_

#include "renderer/font.hpp"
#include "renderer/shader.hpp"
#include "renderer/texture.hpp"
#include "sound/sound_assets.hpp"

#include <filesystem>

#include <audeo/audeo.hpp>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace munchkin {
namespace assets {
namespace loaders {

namespace impl {
struct SinglePathLoadParams {
    static SinglePathLoadParams from_json(nlohmann::json json_object, fs::path root_path) {
        if (!json_object.is_string())
            return {""};
        SinglePathLoadParams obj;
        obj.path = root_path / static_cast<std::string>(json_object);
        return obj;
    }
    operator bool() const { return path != ""; }
    fs::path path;
};
struct ShaderLoadParams {
    static ShaderLoadParams from_json(nlohmann::json json_object, fs::path root_path) {
        if (!json_object["vert"].is_string() || !json_object["frag"].is_string())
            return {"", ""};
        ShaderLoadParams obj;
        obj.vert = root_path / static_cast<std::string>(json_object["vert"]);
        obj.frag = root_path / static_cast<std::string>(json_object["frag"]);
        return obj;
    }
    operator bool() const { return vert != "" && frag != ""; }
    fs::path vert;
    fs::path frag;
};
} // namespace impl

template<typename T> struct LoadParams;

template<> struct LoadParams<renderer::Texture> : public impl::SinglePathLoadParams {};
void load(renderer::Texture& texture, LoadParams<renderer::Texture> const& params);

template<> struct LoadParams<renderer::Shader> : public impl::ShaderLoadParams {};
void load(renderer::Shader& shader, LoadParams<renderer::Shader> const& params);

template<> struct LoadParams<renderer::Font> : public impl::SinglePathLoadParams {};
void load(renderer::Font& font, LoadParams<renderer::Font> const& params);

template<> struct LoadParams<sound::SoundEffect> : public impl::SinglePathLoadParams {};
void load(sound::SoundEffect& sound, LoadParams<sound::SoundEffect> const& params);

template<> struct LoadParams<sound::Music> : public impl::SinglePathLoadParams {};
void load(sound::Music& music, LoadParams<sound::Music> const& params);

void free(renderer::Texture& texture);
void free(renderer::Shader& shader);
inline void free(renderer::Font& font) {} /* This is taken care of by the Font destructor */
inline void free(sound::SoundEffect& sound) {
} /* This is taken care of by the SoundSource destructor */
inline void free(sound::Music& music) {} /* This is taken care of by the SoundSource destructor */

} // namespace loaders

} // namespace assets
} // namespace munchkin

#endif