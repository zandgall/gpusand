#include "quickwork_io.h"
#include "quickwork.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::map<std::string, unsigned int> shader_cache = std::map<std::string, unsigned int>(), texture_cache = std::map<std::string, unsigned int>(), compute_cache = std::map<std::string, unsigned int>();

unsigned int qwio::compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char*)malloc(length);
        glGetShaderInfoLog(id, length, &length, message);
        std::string line;
        std::istringstream stream(source);
        int lineNum = 0;
        while(getline(stream, line))
            std::cout << ++lineNum << ": " << line << std::endl;
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : type == GL_FRAGMENT_SHADER ? "fragment" : "compute") << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        free(message);
        return 0;
    }
    return id;
}

unsigned int qwio::load_shader(const char* path) {
    std::ifstream stream(path);
    
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while(getline(stream, line)) {
        if(line.find("#shader") != std::string::npos) {
            if(line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else if(type != ShaderType::NONE)
            ss[(size_t)type] << line << '\n';
    }

    stream.close();

    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, ss[0].str());
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, ss[1].str());

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int qwio::get_loaded_shader(const char* name) {
    return shader_cache[std::string(name)];
}

void qwio::load_shader_as(const char* path, const char* name) {
    shader_cache[std::string(name)] = load_shader(path);
}

unsigned int qwio::load_compute(const char* path) {
    std::ifstream stream(path);
    std::string line;
    std::stringstream ss;
    while(getline(stream, line)) {
        ss << line << '\n';
    }

    stream.close();

    unsigned int program = glCreateProgram();
    unsigned int cs = compileShader(GL_COMPUTE_SHADER, ss.str());

    glAttachShader(program, cs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(cs);

    return program;
}

unsigned int qwio::get_loaded_compute(const char* name) {
    return compute_cache[std::string(name)];
}

void qwio::load_compute_as(const char* path, const char* name) {
    compute_cache[std::string(name)] = load_compute(path);
}

unsigned int qwio::load_texture(const char* path, int magFilter, int minFilter, int wrapS, int wrapT) {
    GLuint texture;
    glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cerr << "Couldn't find texture \"" << path << "\"" << std::endl;
	stbi_image_free(data);
	return texture;
}

unsigned int qwio::get_loaded_texture(const char* name) {
	return texture_cache[std::string(name)];
}

void qwio::load_texture_as(const char* path, const char* name, int magFilter, int minFilter, int wrapS, int wrapT) {
	texture_cache[std::string(name)] = load_texture(path, magFilter, minFilter, wrapS, wrapT);
}

void qwio::load_font_as(const char* path, const char* name, int font_size, unsigned long glyphs_to_load) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    qw::font font;

    if(FT_New_Face(qw::freetype_library, path, 0, &font.face)) {
        std::cerr << "Could not load font " << path << " (" << name << ")" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(font.face, 0, font_size);
    font.size = font_size;
    font.glyphs = std::map<unsigned long, qw::glyph>();

    for(unsigned long c = 0; c < glyphs_to_load; c++) {
        if(FT_Load_Char(font.face, c, FT_LOAD_RENDER)) {
            std::cerr << "Couldn't load char #" << c << " (" << name << ")" << std::endl;
            return;
        }

        qw::glyph glyph;

        glyph.size = glm::vec2(font.face->glyph->bitmap.width, font.face->glyph->bitmap.rows);
        glyph.bearing = glm::vec2(font.face->glyph->bitmap_left, font.face->glyph->bitmap_top);
        glyph.glyph_index = font.face->glyph->glyph_index;
        glyph.advance = font.face->glyph->advance.x;
        

        glGenTextures(1, &glyph.texture);
        glBindTexture(GL_TEXTURE_2D, glyph.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph.size.x, glyph.size.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, font.face->glyph->bitmap.buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_ALPHA };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

        font.glyphs[c] = glyph;
    }

    qw::fonts[std::string(name)] = font;
}
