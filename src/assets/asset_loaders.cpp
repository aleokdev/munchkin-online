#include "assets/asset_loaders.hpp"

#include "renderer/util.hpp"
#include "sound/sound_assets.hpp"

#include <fstream>
#include <glad/glad.h>
#include <stb/stb_image.h>

using namespace munchkin::renderer;

namespace munchkin {
namespace assets {
namespace loaders {

void load(Texture& texture, LoadParams<Texture> const& params) {
    stbi_set_flip_vertically_on_load(true);
    int w, h, channels;
    unsigned char* data = stbi_load(params.path.generic_string().c_str(), &w, &h, &channels, 4);

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    texture.handle = tex;
    texture.w = w;
    texture.h = h;
}

void load(Shader& shader, LoadParams<Shader> const& params) {
    shader.handle = renderer::load_shader(params.vert.generic_string().c_str(),
                                          params.frag.generic_string().c_str());
}

void load(Font& font, LoadParams<Font> const& params) {
    // Load and swap
    Font f(params.path);
    font.swap(f);
}

void load(sound::SoundEffect& sound, LoadParams<sound::SoundEffect> const& params) {
    std::string path_str = params.path.string();
    sound.source = audeo::load_source(path_str, audeo::AudioType::Effect);
}

void load(sound::Music& music, LoadParams<sound::Music> const& params) {
    std::string path_str = params.path.string();
    music.source = audeo::load_source(path_str, audeo::AudioType::Music);

    // TODO: Get taglib working, or get actual metadata from song files
    music.title = params.path.stem().generic_string();
    music.artist = "Kevin Macleod";
}

void free(Texture& texture) { glDeleteTextures(1, &texture.handle); }

void free(Shader& shader) { glDeleteProgram(shader.handle); }

} // namespace loaders
} // namespace assets
} // namespace munchkin