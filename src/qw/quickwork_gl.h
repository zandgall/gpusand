#pragma once
#ifndef QUICKWORK_GL_H
#define QUICKWORK_GL_H
#include <glm/glm.hpp>
namespace qwgl {
    // Uniform functions
    extern void uniform(const char* name, const float value);
    extern void uniform(const char* name, const int value);
    extern void uniform(const char* name, const unsigned int value);
    extern void uniform(const char* name, const double value);
    extern void uniform(const char* name, const bool value);
    extern void uniform(const char* name, const glm::fvec2 value);
    extern void uniform(const char* name, const glm::fvec3 value);
    extern void uniform(const char* name, const glm::fvec4 value);
    extern void uniform(const char* name, const glm::ivec2 value);
    extern void uniform(const char* name, const glm::ivec3 value);
    extern void uniform(const char* name, const glm::ivec4 value);
    extern void uniform(const char* name, const glm::dvec2 value);
    extern void uniform(const char* name, const glm::dvec3 value);
    extern void uniform(const char* name, const glm::dvec4 value);
    extern void uniform(const char* name, const glm::fmat2x2 value);
    extern void uniform(const char* name, const glm::fmat2x3 value);
    extern void uniform(const char* name, const glm::fmat2x4 value);
    extern void uniform(const char* name, const glm::fmat3x2 value);
    extern void uniform(const char* name, const glm::fmat3x3 value);
    extern void uniform(const char* name, const glm::fmat3x4 value);
    extern void uniform(const char* name, const glm::fmat4x2 value);
    extern void uniform(const char* name, const glm::fmat4x3 value);
    extern void uniform(const char* name, const glm::fmat4x4 value);
    extern void uniform(const char* name, const glm::dmat2x2 value);
    extern void uniform(const char* name, const glm::dmat2x3 value);
    extern void uniform(const char* name, const glm::dmat2x4 value);
    extern void uniform(const char* name, const glm::dmat3x2 value);
    extern void uniform(const char* name, const glm::dmat3x3 value);
    extern void uniform(const char* name, const glm::dmat3x4 value);
    extern void uniform(const char* name, const glm::dmat4x2 value);
    extern void uniform(const char* name, const glm::dmat4x3 value);
    extern void uniform(const char* name, const glm::dmat4x4 value);

    // Helpful transformation functions
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Create a transformation using a given position, rotation, and scale. Builds off of given matrix
    extern glm::mat4 transformation(glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1), glm::mat4 matrix = glm::mat4(1));

    // Create a transformation that represents a 2 dimensional rectangle
    extern glm::mat4 rectangle(float x, float y, float w, float h);

    // Create a transformation that represents a line between two points, with a width
    extern glm::mat4 line_2d(glm::vec2 a, glm::vec2 b, float thickness = 10);
}
#endif