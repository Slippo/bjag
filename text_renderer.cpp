#include "text_renderer.h"
#include <iostream>

namespace game {

    TextRenderer::TextRenderer() {
        
        // Load freetype library
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "Error: Could not load freetype library" << "\n";
        }

        // Load font
        FT_Face face;
        if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
            std::cout << "ERROR: Could not load font" << "\n";
        }

        // Load all character glyphs
        FT_Set_Pixel_Sizes(face, 0, 48);

        for (unsigned char c = 0; c < 128; c++)
        {
            // Load current glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "Error: Could not load glyph" << "\n";
                continue;
            }

            // Glyph texture setup
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // Texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Add glyph as character struct to list for easy use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            characters_.insert(std::pair<char, Character>(c, character));
        }

        // Free resources
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // freetype setup
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

        // Setup quads
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
    }

    TextRenderer::~TextRenderer() {};

    void TextRenderer::RenderText(GLuint shader, glm::vec2 position, float scale, glm::vec3 color, std::string text) {

        // Select proper material (shader program)
        glUseProgram(shader);

        // Send data to text shader
        glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // Render each character in text
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = characters_[*c];

            float xpos = position.x + ch.Offset.x * scale;
            float ypos = position.y - (ch.Size.y - ch.Offset.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Advance to next glyph + spacing
            position.x += (ch.Spacing >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}