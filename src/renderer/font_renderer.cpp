#include "renderer/font_renderer.hpp"
#include "renderer/font.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin {
	namespace renderer {

		FontRenderer::FontRenderer() {
			setup_for_render();
		}

		void FontRenderer::deallocate() {
			if (vao) {
				glDeleteVertexArrays(1, &vao);
			}
			if (vbo) {
				glDeleteVertexArrays(1, &vbo);
			}
		}

		void FontRenderer::setup_for_render() {
			if (!vao || !vbo) {
				init();
			}

			glBindVertexArray(vao);
		}

		void FontRenderer::init() {
			static float vertices[] = {
				// positions    texcoords
				-.5f, -.5f,         0, 0,
				-.5, .5,          0, 1,
				.5, -.5,          1, 0,
				-.5, .5,          0, 1,
				.5, -.5,          1, 0,
				.5, .5,           1, 1
			};

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			// Fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Setup vao
			glBindVertexArray(vao);

			// Positions
			glEnableVertexAttribArray(0);
			glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
			glBindVertexBuffer(0, vbo, 0, 4 * sizeof(float));
			glVertexAttribBinding(0, 0);

			// TexCoords
			glEnableVertexAttribArray(1);
			glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
			glBindVertexBuffer(1, vbo, 0, 4 * sizeof(float));
			glVertexAttribBinding(1, 1);
		}

		/*static void FontRenderer::render_glyph(Font::glyph_data const& data, glm::vec2 offset) {

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, data.texture);
			shader.set_int(Shader::Uniforms::Texture, 0);
			glm::vec2 img_dim = glm::vec2(data.size.x, data.size.y);
			glm::vec2 target_dim = glm::vec2(target.get_size().x, target.get_size().y);
			shader.set_vec2(0, glm::vec2(img_dim.x / target_dim.x * size.x,
				img_dim.y / target_dim.y * size.y));
			glUniform2fv(1, 1, glm::value_ptr(position));
			glUniform2fv(2, 1, glm::value_ptr(offset));

			glDrawElements(GL_TRIANGLES, quad.index_size(), GL_UNSIGNED_INT, nullptr);
		}*/

	}
}