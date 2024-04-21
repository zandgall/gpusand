#pragma once
#ifndef QUICKWORK_H
#define QUICKWORK_H

#include <glad/gl.h>
#include <KHR/khrplatform.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>

namespace qw {

    struct glyph {
        unsigned int texture;
        glm::vec2 bearing, size;
        signed long advance; // Only measures in x direction
        FT_UInt glyph_index; // Can be used for kerning
    };

    struct font {
        std::map<unsigned long, glyph> glyphs;
        FT_Face face;
        // Height of font
        int size;
    };

    inline std::map<std::string, font> fonts = std::map<std::string, font>();

    inline FT_Library freetype_library;

    // Starts an OpenGL Window with GLFW3 and Glad. Hooks it to callbacks that update qw variables, like 'mouse_position', 'width'/'height', and 'keys'.
    extern int start(const char* window_name = "Window", int width = 1280, int height = 720, int gl_version_major = 4, int gl_version_minor = 6);

    // Boolean that represents whether the qw::window should close or not. Can be used to continuously run program while window is open.
    inline bool should_close = false;

    // Keeps the width and height of the qw::window. Is updated on window rescale.
    inline int width = 1, height = 1;

    // The position of the mouse.
    inline glm::vec2 mouse_position(0);
    // The position of the mouse last frame.
    inline glm::vec2 p_mouse_position(0);
    // The velocity of the mouse.
    inline glm::vec2 mouse_velocity(0);

    // The scroll (horizontal and vertical) of the mouse.
    inline glm::vec2 scroll(0);
    // The scroll (horizontal and vertical) of the mouse last frame.
    inline glm::vec2 p_scroll(0);

    // Whether or not the mouse button is pressed.
    inline bool mouse_left = false, mouse_middle = false, mouse_right = false;
    // What the previous state of the mouse button was.
    inline bool p_mouse_left = false, p_mouse_middle = false, p_mouse_right = false;

    // An array of key states. True if currently pressed, false if not pressed.
    inline bool keys[GLFW_KEY_LAST+1];
    // Array of previous key states. What the state of each key was last frame.
    inline bool p_keys[GLFW_KEY_LAST+1];
    // Store the name of the last key typed 
    inline std::string last_typed_key = "No key pressed";

    // The amount of frames counted in a whole second. Updated at the end of every second.
    inline int frames_last_second = 0;
    // The change in time between the beginning of the last friend, and the beginning of this one.
    inline double delta_time = 0;
    // The amount of frames to happen in one second if each frame lasted the amount of time that delta_time currently is. (Computation is in the name.)
    inline double one_over_delta_time = 0;
    // Used in the calculation of average_frame_rate. Constitutes how much of the average is retained every frame.
    // A weight of 0 would set average_frame_rate to the current one_over_delta_time every frame.
    // A weight of 1 would keep average_frame_rate the same value no matter what.
    inline double average_frame_smoothing_weight = 0.9;
    // A running average of frame rates. Uses one_over_delta_time, as well as average_frame_smoothing_weight.
    inline double average_frame_rate = 0;
    // Stores the difference between the current one_over_delta_time and average_frame_rate. High values of volatility may signal that a lag spike is happening, has happened, or will happen.
    inline double frame_volatility = 0;

    // Uses GLFW3 to poll Window events. Gathering frame information in the same function.
    extern void poll();


    // Draw a simple square. Can use transformations (like rectangle or lineto) to draw any quad
    extern void draw_square();

    enum text {
        ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
    };

    // Get the width of a string. 
    extern float measure(std::string, std::string font, float font_weight);

    // Get the maximum distance up and down from the baseline of a string. 
    extern glm::vec2 measure_y_span(std::string, std::string font, float font_weight);

    // Draw a string, using the given font and font size. X and Y are the beginning point. Y is the base line, and 'alignment' determines whether X is the left of the string, right of the string, or center of the string.
    extern void draw_string(std::string string, std::string font, float font_weight, float x, float y, qw::text alignment = ALIGN_LEFT);

    // Draw a string onto a predefined surface, using the given font. The size of the text will be stretched to fit the given surface. Surface is as if it's a transformation of a rectangle.
    extern void draw_string_on(std::string string, std::string font, glm::mat4 surface);
}
#endif