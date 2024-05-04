#include "qw/quickwork.h"
#include "qw/quickwork_io.h"
#include "qw/quickwork_gl.h"
#include "qw/quickwork_ui.h"
// #include "qw/quickwork_au.h"
#include "rule.h"
#include "color.h"

#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

struct world {
    unsigned int color, dead, alive, physic_props;
} wrld;
unsigned int world_dead_buff;

unsigned int noise;

int frame, selected_element = 0;

void sand_key_call(GLFWwindow* window, int key, int scancode, int action, int mods) {
    qw::key_call(window, key, scancode, action, mods);
    std::cout << key << " pressed: \'" << (char)key << "'\n";
    if(rules::element_keymap.find((char)key)!=rules::element_keymap.end()) {
        selected_element = rules::element_keymap[(char)key];
        std::cout << "mapping " << selected_element << std::endl;
    }
}

int main(int args, char** argv) {
    if (qw::start("More Sand", 1024, 1024))// If startup fails, exit with return code -1
        return -1;
    glfwSetKeyCallback(qw::window, sand_key_call);
    // if (qwau::start()) // If audio fails, exit with return code -1
        // return -1;

    qwio::load_shader_as("res/shaders/shader.shader", "default shader");
    glUseProgram(qwio::get_loaded_shader("default shader"));
    qwgl::uniform("text", 1);
    qwgl::uniform("dead", 0);
    qwio::load_texture_as("res/image.png", "default image");
    qwio::load_font_as("res/fonts/robotomono.ttf", "roboto", 96);
    qwio::load_font_as("res/fonts/basicbit3.ttf", "basicbit", 96);

    qwio::load_compute_as("res/shaders/fallablesfall.shader", "fallables fall");
    qwio::load_compute_as("res/shaders/piling.shader", "piling");
    
    glGenTextures(4, &wrld.color);
    glBindTexture(GL_TEXTURE_2D, wrld.color);
    unsigned char color_dat[64 * 64 * 4];
    for(int i = 0; i < 64 * 64 * 4; i+=4) {
        color_dat[i+0] = 155;
        color_dat[i+1] = 215;
        color_dat[i+2] = 232;
        color_dat[i+3] = 255;
    }
    for(int i = 0; i < 64; i++) 
        color_dat[64*64*4 + i * 4 + 1] = 0;
    for(int i = 2; i < 32; i+=2) {
        color_dat[32 * 4 + i*64*4 + 0] = 255;
        color_dat[32 * 4 + i*64*4 + 1] = 0;
        color_dat[32 * 4 + i*64*4 + 2] = 0;
        color_dat[32 * 4 + i*64*4 + 3] = 255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(1, wrld.color, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    unsigned int dead_dat[64 * 64];
    for(int i = 0; i < 64 * 64; i++) {
        dead_dat[i] = 0;
    }
    for(int i = 0; i < 64; i++)
        dead_dat[63*64 + i] = 2;
    for(int i = 2; i < 32; i+=2)
        dead_dat[32 + i*64] = 1;
    glBindTexture(GL_TEXTURE_2D, wrld.dead);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 64, 64, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, dead_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(0, wrld.dead, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
    
    uint8_t noise_dat[1024*4];
    srand(time(NULL));
    for(int i = 0; i < 1024 * 4; i++)
        noise_dat[i] = rand()%256;
    while(glGetError());
    glGenTextures(1, &noise);
    glBindTexture(GL_TEXTURE_1D, noise);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glBindImageTexture(5, noise, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glGenBuffers(1, &world_dead_buff);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, world_dead_buff);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64*64*sizeof(unsigned int), dead_dat, GL_DYNAMIC_DRAW);

    glUseProgram(qwio::get_loaded_compute("fallables fall"));
    qwgl::uniform("dead", 0);
    qwgl::uniform("world_color", 1);
    qwgl::uniform("non_solid", 2);
    qwgl::uniform("gravity_affected", 3);

    glUseProgram(qwio::get_loaded_compute("piling"));
    qwgl::uniform("world_color", 1);
    // qwgl::uniform("world_dead", 1);
    qwgl::uniform("fallable", 2);
    qwgl::uniform("passthrough", 3);
    qwgl::uniform("solid", 4);
    qwgl::uniform("noise", 5);


    rules::loadRuleset("res/rules/sand.ruleset");

    rules::applyRuleset(noise);

    glBindImageTexture(2, rules::properties["passthrough"], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(3, rules::properties["fallable"], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(4, rules::properties["solid"], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    glBindImageTexture(5, noise, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    while(!qw::should_close) {

        if(qw::mouse_left && qw::mouse_position.x >= 0 && qw::mouse_position.x < qw::width && qw::mouse_position.y >= 0 && qw::mouse_position.y < qw::height) {
            glBindTexture(GL_TEXTURE_2D, wrld.color);
            glm::vec3 col = toRgb(toHsv(rules::elements[selected_element].color)+rules::elements[selected_element].hsv_variance*glm::vec3((float)rand()/RAND_MAX-0.5,(float)rand()/RAND_MAX-0.5,(float)rand()/RAND_MAX-0.5));
            uint8_t painted_color[4];
            painted_color[0] = col.x*255;
            painted_color[1] = col.y*255;
            painted_color[2] = col.z*255;
            painted_color[3] = 255;
            glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(qw::mouse_position.x*64/qw::width), (int)(qw::mouse_position.y*64/qw::height), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, painted_color);
            uint32_t painted_dead[1];
            painted_dead[0] = selected_element;
            glBindTexture(GL_TEXTURE_2D, wrld.dead);
            glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(qw::mouse_position.x*64/qw::width), (int)(qw::mouse_position.y*64/qw::height), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, painted_dead);
        }

        glViewport(0, 0, qw::width, qw::height);
        glClearColor(0.5, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);


        // glUseProgram(qwio::get_loaded_compute("fallables fall"));
        // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, world_dead_buff);
        // glDispatchCompute(64 / 16, 64 / 16, 1);
        // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // glUseProgram(qwio::get_loaded_compute("piling"));
        // qwgl::uniform("frame", frame);
        // glDispatchCompute(256 / 16, 256 / 16, 1);
        // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        for(int i = 0; i < rules::ruleset.size(); i++)
            rules::ruleset[i].run(noise, world_dead_buff);

        glUseProgram(qwio::get_loaded_shader("default shader"));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, world_dead_buff);

        qwgl::uniform("screenspace", glm::ortho<float>(0.f, qw::width, qw::height, 0.f, 0.f, qw::width*2));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, wrld.color);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, wrld.dead);
        glActiveTexture(GL_TEXTURE0);
        qwgl::uniform("transform", qwgl::rectangle(0, 0, qw::width, qw::height));
        qw::draw_square();

        qw::poll();
        frame++;
        frame %= 1024;

        auto start = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(start-start);
        do {
            auto stop = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(stop-start);
        } while(duration.count()<400000);
    }
}