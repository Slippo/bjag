#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

/*
The TextRenderer class sets up and renders text.
*/

#include "game.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/freetype.h"
#include <map>

namespace game {
	class TextRenderer {
	public:
		TextRenderer();
		~TextRenderer();

		void RenderText(GLuint shader, glm::vec2 position, float scale, glm::vec3 color, std::string text);

		struct Character {
			unsigned int TextureID; // ID of glyph
			glm::ivec2 Size; // Size of glyph
			glm::ivec2 Offset; // Left/right offset of glyph
			unsigned int Spacing; // Spacing before next glyph
		};

	private:

		unsigned int VAO;
		unsigned int VBO;

		std::map<char, Character> characters_;
	};
}

#endif