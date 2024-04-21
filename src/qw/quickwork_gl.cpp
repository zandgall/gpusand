#include "quickwork.h"
#include "quickwork_gl.h"
#include <glm/gtc/matrix_transform.hpp>

// Uniform helper, finds location of a uniform with 'name' in the current shader program
int __loc(const char* name) {
    GLint shader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    return glGetUniformLocation(shader, name);
}

// Uniform functions

void qwgl::uniform(const char* name, const float value) {glUniform1f(__loc(name), value);}
void qwgl::uniform(const char* name, const int value) {glUniform1i(__loc(name), value);}
void qwgl::uniform(const char* name, const unsigned int value) {glUniform1ui(__loc(name), value);}
void qwgl::uniform(const char* name, const double value) {glUniform1d(__loc(name), value);}
void qwgl::uniform(const char* name, const bool value) {glUniform1i(__loc(name), value);}
void qwgl::uniform(const char* name, const glm::fvec2 value) {glUniform2f(__loc(name), value.x, value.y);}
void qwgl::uniform(const char* name, const glm::fvec3 value) {glUniform3f(__loc(name), value.x, value.y, value.z);}
void qwgl::uniform(const char* name, const glm::fvec4 value) {glUniform4f(__loc(name), value.x, value.y, value.z, value.w);}
void qwgl::uniform(const char* name, const glm::ivec2 value) {glUniform2i(__loc(name), value.x, value.y);}
void qwgl::uniform(const char* name, const glm::ivec3 value) {glUniform3i(__loc(name), value.x, value.y, value.z);}
void qwgl::uniform(const char* name, const glm::ivec4 value) {glUniform4i(__loc(name), value.x, value.y, value.z, value.w);}
void qwgl::uniform(const char* name, const glm::dvec2 value) {glUniform2d(__loc(name), value.x, value.y);}
void qwgl::uniform(const char* name, const glm::dvec3 value) {glUniform3d(__loc(name), value.x, value.y, value.z);}
void qwgl::uniform(const char* name, const glm::dvec4 value) {glUniform4d(__loc(name), value.x, value.y, value.z, value.w);}
void qwgl::uniform(const char* name, const glm::fmat2x2 value) {glUniformMatrix2fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat2x3 value) {glUniformMatrix2x3fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat2x4 value) {glUniformMatrix2x4fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat3x2 value) {glUniformMatrix3x2fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat3x3 value) {glUniformMatrix3fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat3x4 value) {glUniformMatrix3x4fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat4x2 value) {glUniformMatrix4x2fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat4x3 value) {glUniformMatrix4x3fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::fmat4x4 value) {glUniformMatrix4fv(__loc(name), 1, GL_FALSE, (const float*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat2x2 value) {glUniformMatrix2dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat2x3 value) {glUniformMatrix2x3dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat2x4 value) {glUniformMatrix2x4dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat3x2 value) {glUniformMatrix3x2dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat3x3 value) {glUniformMatrix3dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat3x4 value) {glUniformMatrix3x4dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat4x2 value) {glUniformMatrix4x2dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat4x3 value) {glUniformMatrix4x3dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}
void qwgl::uniform(const char* name, const glm::dmat4x4 value) {glUniformMatrix4dv(__loc(name), 1, GL_FALSE, (const double*)&(value[0]));}

glm::mat4 qwgl::transformation(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::mat4 mat) {
    mat = glm::translate(mat, pos);
    mat = glm::rotate(mat, rot.x, glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, rot.y, glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, rot.z, glm::vec3(0, 0, 1));
    return glm::scale(mat, scale);
}

glm::mat4 qwgl::rectangle(float x, float y, float w, float h) {
    return transformation(glm::vec3(x + w / 2.f, y + h/2.f, 0), glm::vec3(0), glm::vec3(w / 2.f, h / 2.f, 0));
}

glm::mat4 qwgl::line_2d(glm::vec2 a, glm::vec2 b, float thickness) {
    glm::vec2 down = b - a;
    glm::vec2 right = glm::normalize(glm::cross(glm::vec3(down, 0), glm::vec3(0, 0, 1)).xy()) * thickness;
    glm::mat4 out = glm::mat4(glm::vec4(right.x, right.y, 0, 0), glm::vec4(down.x, down.y, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4((a.x+b.x)/2, (a.y+b.y)/2, 0, 1));
    return out;
}