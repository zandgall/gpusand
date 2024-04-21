#include "quickwork.h"
#include "quickwork_gl.h"
#include <iostream>

// Local (file scope) variable definitions

GLFWwindow* window;
unsigned int square_vao = 0, square_vbo = 0, square_ebo = 0;
void set_up_square() {
    // Square VAO definitions 
    float vertices[] = {
      1.0f,  1.0f, 0.0f, 1, 1, 1, 0, 0, -1, // top right
      1.0f, -1.0f, 0.0f, 1, 1, 0, 0, 0, -1, // bottom right
     -1.0f, -1.0f, 0.0f, 1, 0, 0, 0, 0, -1, // bottom left
     -1.0f,  1.0f, 0.0f, 1, 0, 1, 0, 0, -1  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    glGenVertexArrays(1, &square_vao);
    glBindVertexArray(square_vao);

    glGenBuffers(1, &square_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
    glBufferData(GL_ARRAY_BUFFER, 36*sizeof(float), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &square_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, square_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(4*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), (GLvoid*)(6*sizeof(float)));
}

// List of key names, spelt out. i.e. "Space" instead of " "
std::string key_name_map[GLFW_KEY_LAST+1];

void set_up_key_names() {
    for (int i = 0; i < GLFW_KEY_LAST; i++)
        if(glfwGetKeyName(i, 0))
            key_name_map[i] = std::string("") + (char)toupper(glfwGetKeyName(i, 0)[0]);
        else
            key_name_map[i] = "";
    key_name_map[GLFW_KEY_SPACE] = "Space";
    key_name_map[GLFW_KEY_ESCAPE] = "Esc";
    key_name_map[GLFW_KEY_ENTER] = "Enter";
    key_name_map[GLFW_KEY_TAB] = "Tab";
    key_name_map[GLFW_KEY_BACKSPACE] = "Backspace";
    key_name_map[GLFW_KEY_INSERT] = "Insert";
    key_name_map[GLFW_KEY_DELETE] = "Delete";
    key_name_map[GLFW_KEY_RIGHT] = "Right";
    key_name_map[GLFW_KEY_LEFT] = "Left";
    key_name_map[GLFW_KEY_DOWN] = "Down";
    key_name_map[GLFW_KEY_UP] = "Up";
    key_name_map[GLFW_KEY_PAGE_UP] = "Page Up";
    key_name_map[GLFW_KEY_PAGE_DOWN] = "Page Down";
    key_name_map[GLFW_KEY_HOME] = "Home";
    key_name_map[GLFW_KEY_END] = "End";
    key_name_map[GLFW_KEY_CAPS_LOCK] = "Caps Lock";
    key_name_map[GLFW_KEY_SCROLL_LOCK] = "Scroll Lock";
    key_name_map[GLFW_KEY_NUM_LOCK] = "Num Lock";
    key_name_map[GLFW_KEY_PRINT_SCREEN] = "Print Screen";
    key_name_map[GLFW_KEY_PAUSE] = "Pause";
    key_name_map[GLFW_KEY_F1] = "F1";
    key_name_map[GLFW_KEY_F2] = "F2";
    key_name_map[GLFW_KEY_F3] = "F3";
    key_name_map[GLFW_KEY_F4] = "F4";
    key_name_map[GLFW_KEY_F5] = "F5";
    key_name_map[GLFW_KEY_F6] = "F6";
    key_name_map[GLFW_KEY_F7] = "F7";
    key_name_map[GLFW_KEY_F8] = "F8";
    key_name_map[GLFW_KEY_F9] = "F9";
    key_name_map[GLFW_KEY_F10] = "F10";
    key_name_map[GLFW_KEY_F11] = "F11";
    key_name_map[GLFW_KEY_F12] = "F12";
    key_name_map[GLFW_KEY_F13] = "F13";
    key_name_map[GLFW_KEY_F14] = "F14";
    key_name_map[GLFW_KEY_F15] = "F15";
    key_name_map[GLFW_KEY_F16] = "F16";
    key_name_map[GLFW_KEY_F17] = "F17";
    key_name_map[GLFW_KEY_F18] = "F18";
    key_name_map[GLFW_KEY_F19] = "F19";
    key_name_map[GLFW_KEY_F20] = "F20";
    key_name_map[GLFW_KEY_F21] = "F21";
    key_name_map[GLFW_KEY_F22] = "F22";
    key_name_map[GLFW_KEY_F23] = "F23";
    key_name_map[GLFW_KEY_F24] = "F24";
    key_name_map[GLFW_KEY_F25] = "F25";
    key_name_map[GLFW_KEY_KP_0] = "Numpad 0";
    key_name_map[GLFW_KEY_KP_1] = "Numpad 1";
    key_name_map[GLFW_KEY_KP_2] = "Numpad 2";
    key_name_map[GLFW_KEY_KP_3] = "Numpad 3";
    key_name_map[GLFW_KEY_KP_4] = "Numpad 4";
    key_name_map[GLFW_KEY_KP_5] = "Numpad 5";
    key_name_map[GLFW_KEY_KP_6] = "Numpad 6";
    key_name_map[GLFW_KEY_KP_7] = "Numpad 7";
    key_name_map[GLFW_KEY_KP_8] = "Numpad 8";
    key_name_map[GLFW_KEY_KP_9] = "Numpad 9";
    key_name_map[GLFW_KEY_KP_DECIMAL] = "Numpad .";
    key_name_map[GLFW_KEY_KP_DIVIDE] = "Numpad /";
    key_name_map[GLFW_KEY_KP_MULTIPLY] = "Numpad *";
    key_name_map[GLFW_KEY_KP_SUBTRACT] = "Numpad -";
    key_name_map[GLFW_KEY_KP_ADD] = "Numpad +";
    key_name_map[GLFW_KEY_KP_ENTER] = "Numpad Enter";
    key_name_map[GLFW_KEY_KP_EQUAL] = "Numpad =";
    key_name_map[GLFW_KEY_LEFT_SHIFT] = "Left Shift";
    key_name_map[GLFW_KEY_LEFT_CONTROL] = "Left Control";
    key_name_map[GLFW_KEY_LEFT_ALT] = "Left Alt";
    key_name_map[GLFW_KEY_LEFT_SUPER] = "Left Super";
    key_name_map[GLFW_KEY_RIGHT_SHIFT] = "Right Shift";
    key_name_map[GLFW_KEY_RIGHT_CONTROL] = "Right Control";
    key_name_map[GLFW_KEY_RIGHT_ALT] = "Right Alt";
    key_name_map[GLFW_KEY_RIGHT_SUPER] = "Right Super";
    key_name_map[GLFW_KEY_MENU] = "Menu";
}

// Callback declarations

void APIENTRY _glDebugOutput(GLenum, GLenum, unsigned int, GLenum, GLsizei, const char*, const void*);
void framebuffer_size_call(GLFWwindow* window, int width, int height);
void cursor_pos_call(GLFWwindow* window, double x, double y);
void mouse_button_call(GLFWwindow* window, int button, int action, int mods);
void key_call(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_call(GLFWwindow* window, double x, double y);

// qw external methods definitions

int qw::start(const char* name, int width, int height, int gl_version_major, int gl_version_minor) {

    // GLFW Initiation sequence
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(width, height, name, NULL, NULL);
    qw::width = width;
    qw::height = height;

    if(window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Couldn't load Glad." << std::endl;
        return -1;
    }

#ifdef DEBUG
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

    // Callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_call);
    glfwSetCursorPosCallback(window, cursor_pos_call);
    glfwSetMouseButtonCallback(window, mouse_button_call);
    glfwSetKeyCallback(window, key_call);
    glfwSetScrollCallback(window, scroll_call);

    // Freetype
    if(FT_Init_FreeType(&freetype_library)) {
        std::cerr << "Could not initiate FreeType." << std::endl;
        return -1;
    }

    set_up_square();
    set_up_key_names();

    // Enable some default OpenGL Settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    return 0;
}

double last_time = glfwGetTime();

int n_frames = 0;

void qw::poll() {
    n_frames++;
    // Set last-frame input variables.
    std::copy(keys, keys+GLFW_KEY_LAST-1, p_keys);
    p_mouse_left = mouse_left;
    p_mouse_middle = mouse_middle;
    p_mouse_right = mouse_right;
    p_mouse_position = mouse_position;
    p_scroll = scroll;

    glfwSwapBuffers(window);
    glfwPollEvents();
    should_close = glfwWindowShouldClose(window);

    mouse_velocity = mouse_position - p_mouse_position;

    double now = glfwGetTime();

    // If we pass a second mark, (i.e. from 5.9 seconds to 6.0 seconds)
    if(trunc(now) - trunc(last_time) > 0) {
        frames_last_second = n_frames;
        n_frames = 0;
    }

    delta_time = now - last_time;
    one_over_delta_time = 1/delta_time;

    frame_volatility = one_over_delta_time - average_frame_rate;
    average_frame_rate = average_frame_smoothing_weight * average_frame_rate + one_over_delta_time * (1-average_frame_smoothing_weight);

    last_time = now;
}

void qw::draw_square() {
    glBindVertexArray(square_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

float qw::measure(std::string string, std::string font, float font_weight) {
    float width = 0, scale = font_weight / qw::fonts[font].size;

    // Loop through every character, grab its glyph from the font, and increment by
    // its advancement; shifted to the right 6 bits because its a fixed point number.
    // Multiplied by scale, to appease the given font_weight.
    for(std::string::const_iterator c = string.begin(); c != string.end(); c++)
        width += (qw::fonts[font].glyphs[*c].advance >> 6) * scale;
    
    return width;
}

glm::vec2 qw::measure_y_span(std::string string, std::string font, float font_weight) {
    float up = 0, down = 0, scale = font_weight / qw::fonts[font].size;

    // Loop through every character, grab its glyph from the font, and increment by
    // its advancement; shifted to the right 6 bits because its a fixed point number.
    // Multiplied by scale, to appease the given font_weight.
    for(std::string::const_iterator c = string.begin(); c != string.end(); c++) {
        float s = qw::fonts[font].glyphs[*c].size.y;
        float b = qw::fonts[font].glyphs[*c].bearing.y;
        up = std::max(b * scale, up);
        down = std::max((s-b) * scale, down);
    }
    
    return glm::vec2(up, down);
}

void qw::draw_string(std::string string, std::string font, float font_weight, float x, float y, qw::text alignment) {
    float x_pos = x, scale = font_weight / qw::fonts[font].size;

    // Update x_pos to comply with text alignment
    if(alignment == text::ALIGN_CENTER)
        x_pos -= measure(string, font, font_weight) / 2; 
    else if(alignment == text::ALIGN_RIGHT)
        x_pos -= measure(string, font, font_weight);

    /**
     * First, set some OpenGL flags that are the same for every character.
     * Loop through the characters of the string, passing a transformation that
     * uses the bearing and sizeof the character, both scaled by 'scale'.
     * Then bind its texture and draw it.
     * Advance the running x position by the glyph's advancement, we need to
     * shift advancement right 6 bits, because they are fixed point numbers.
    */

    glBindVertexArray(square_vao);
    qwgl::uniform("textured", true);
    glActiveTexture(GL_TEXTURE0);

    for(std::string::const_iterator c = string.begin(); c != string.end(); c++) {
        qw::glyph glyph = qw::fonts[font].glyphs[*c];

        qwgl::uniform("transform", qwgl::rectangle(
            x_pos + glyph.bearing.x * scale, 
            y - glyph.bearing.y * scale, 
            glyph.size.x * scale, 
            glyph.size.y * scale));

        glBindTexture(GL_TEXTURE_2D, glyph.texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        x_pos += (glyph.advance >> 6) * scale;
    }
}

void qw::draw_string_on(std::string string, std::string font, glm::mat4 surface) {
    glm::vec2 y_span = measure_y_span(string, font, fonts[font].size);
    float width = measure(string, font, fonts[font].size);
    float x_pos = -1, y_scale = 1 / (y_span.x+y_span.y), y_pos = y_span.x * y_scale, x_scale = 2 / width;

    /**
     * Similar to the body of "qw::draw_string", the only difference is that the text
     * only spans the x and y range 0-1.
    */

    glBindVertexArray(square_vao);
    qwgl::uniform("textured", true);
    glActiveTexture(GL_TEXTURE0);

    for(std::string::const_iterator c = string.begin(); c != string.end(); c++) {
        qw::glyph glyph = qw::fonts[font].glyphs[*c];

        qwgl::uniform("transform", surface * qwgl::rectangle(
            x_pos + glyph.bearing.x * x_scale, 
            (y_pos - glyph.bearing.y * y_scale)*2-1, 
            glyph.size.x * x_scale, 
            glyph.size.y * y_scale*2));

        glBindTexture(GL_TEXTURE_2D, glyph.texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        x_pos += (glyph.advance >> 6) * x_scale;
    }
}

// Callbacks definitions

void framebuffer_size_call(GLFWwindow* window, int width, int height){
    qw::width = width;
    qw::height = height;
}

void cursor_pos_call(GLFWwindow* window, double x, double y) {
    qw::mouse_position = glm::vec2(x, y);
}

void mouse_button_call(GLFWwindow* window, int button, int action, int mods) {
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        qw::mouse_left = action == GLFW_PRESS;
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        qw::mouse_middle = action == GLFW_PRESS;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        qw::mouse_right = action == GLFW_PRESS;
        break;
    }
}

void key_call(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS) {
        std::string name = key_name_map[key];
        std::string mod_string = "";
        if(mods & GLFW_MOD_CONTROL)
            mod_string += "Control + ";
        if(mods & GLFW_MOD_SHIFT)
            mod_string += "Shift + ";
        if(mods & GLFW_MOD_ALT)
            mod_string += "Alt + ";
        if(name.length() > 0)
            qw::last_typed_key = mod_string + std::string(name);
        else
            qw::last_typed_key = mod_string + "None";
    }
    qw::keys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
}

void scroll_call(GLFWwindow* window, double x, double y) {
    qw::scroll = glm::vec2(x, y);
}

void APIENTRY _glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* user_params) {
    // ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cerr << "---------------" << std::endl;
	std::cerr << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
	} std::cerr << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
	} std::cerr << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
	} std::cerr << std::endl;
	std::cerr << std::endl;
}