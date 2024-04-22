#include "qw/quickwork.h"
#include "qw/quickwork_io.h"
#include "qw/quickwork_gl.h"
#include "qw/quickwork_ui.h"
#include "qw/quickwork_au.h"

#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

struct world {
    unsigned int color, dead, alive, physic_props;
} wrld;

std::vector<unsigned int> rule_shaders;

unsigned int gravity_affected, non_solid, solid;
unsigned int noise;

int frame;

int main(int args, char** argv) {
    if (qw::start("More Sand", 1024, 1024))// If startup fails, exit with return code -1
        return -1;
    // if (qwau::start()) // If audio fails, exit with return code -1
        // return -1;

    qwio::load_shader_as("res/shaders/shader.shader", "default shader");
    qwio::load_texture_as("res/image.png", "default image");
    qwio::load_font_as("res/fonts/robotomono.ttf", "roboto", 96);
    qwio::load_font_as("res/fonts/basicbit3.ttf", "basicbit", 96);
    qwio::load_compute_as("res/shaders/fallablesfall.shader", "fallables fall");
    glUseProgram(qwio::get_loaded_compute("fallables fall"));
    qwgl::uniform("world_color", 0);
    qwgl::uniform("world_dead", 1);
    qwgl::uniform("gravity_affected", 2);
    qwgl::uniform("non_solid", 3);
    qwio::load_compute_as("res/shaders/piling.shader", "piling");
    glUseProgram(qwio::get_loaded_compute("piling"));
    qwgl::uniform("world_color", 0);
    qwgl::uniform("world_dead", 1);
    qwgl::uniform("gravity_affected", 2);
    qwgl::uniform("non_solid", 3);
    qwgl::uniform("solid", 4);
    qwgl::uniform("noise", 5);

    // Idea? // qwgl::create_texture_2D();

    glGenTextures(4, &wrld.color);
    glBindTexture(GL_TEXTURE_2D, wrld.color);
    unsigned char color_dat[256 * 256 * 4];
    for(int i = 0; i < 256 * 256 * 4; i++) {
        color_dat[i] = 255;
    }
    for(int i = 0; i < 256; i++)
        color_dat[255*256*4 + i * 4 + 1] = 0;
    for(int i = 2; i < 32; i+=2) {
        color_dat[128 * 4 + i*256*4 + 0] = 255;
        color_dat[128 * 4 + i*256*4 + 1] = 0;
        color_dat[128 * 4 + i*256*4 + 2] = 0;
        color_dat[128 * 4 + i*256*4 + 3] = 255;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(0, wrld.color, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    unsigned int dead_dat[256 * 256];
    for(int i = 0; i < 256 * 256; i++) {
        dead_dat[i] = 0;
    }
    for(int i = 0; i < 256; i++)
        dead_dat[255*256 + i] = 2;
    for(int i = 2; i < 32; i+=2)
        dead_dat[128 + i*256] = 1;
    glBindTexture(GL_TEXTURE_2D, wrld.dead);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 256, 256, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, dead_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(1, wrld.dead, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    unsigned int gravity_affected_dat[1];
    gravity_affected_dat[0] = 1;
    glGenTextures(1, &gravity_affected);
    glBindTexture(GL_TEXTURE_1D, gravity_affected);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32UI, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, gravity_affected_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(2, gravity_affected, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);

    unsigned int non_solid_dat[1];
    non_solid_dat[0] = 0;
    glGenTextures(1, &non_solid);
    glBindTexture(GL_TEXTURE_1D, non_solid);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32UI, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, non_solid_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(3, non_solid, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);

    unsigned int solid_dat[2];
    solid_dat[0] = 1;
    solid_dat[1] = 2;
    glGenTextures(1, &solid);
    glBindTexture(GL_TEXTURE_1D, solid);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32UI, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, solid_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindImageTexture(4, solid, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
    
    uint8_t noise_dat[1024*4];
    srand(time(NULL));
    for(int i = 0; i < 1024 * 4; i++) {
        noise_dat[i] = rand()%256;
        std::cout << (int)noise_dat[i] << " ";
    }
    std::cout << std::endl;
    while(glGetError());
    glGenTextures(1, &noise);
    glBindTexture(GL_TEXTURE_1D, noise);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, noise_dat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(5, noise, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    
    while(!qw::should_close) {

        if(qw::mouse_left && qw::mouse_position.x >= 0 && qw::mouse_position.x < qw::width && qw::mouse_position.y >= 0 && qw::mouse_position.y < qw::height) {
            while(glGetError());
            int r;
            glBindTexture(GL_TEXTURE_2D, wrld.color);
            uint8_t painted_color[4];
            painted_color[0] = 255;
            painted_color[1] = 0;
            painted_color[2] = 0;
            painted_color[3] = 255;
            glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(qw::mouse_position.x*256/qw::width), (int)(qw::mouse_position.y*256/qw::height), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, painted_color);
            uint32_t painted_dead[1];
            painted_dead[0] = 1;
            glBindTexture(GL_TEXTURE_2D, wrld.dead);
            glTexSubImage2D(GL_TEXTURE_2D, 0, (int)(qw::mouse_position.x*256/qw::width), (int)(qw::mouse_position.y*256/qw::height), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, painted_dead);
            // while((r=glGetError())) {std::cout << "d: " << r << " " << wrld.dead << std::endl;}
        }

        glViewport(0, 0, qw::width, qw::height);
        glClearColor(0.5, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glUseProgram(qwio::get_loaded_compute("fallables fall"));
        glDispatchCompute(256 / 32, 256 / 32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(qwio::get_loaded_compute("piling"));
        qwgl::uniform("frame", frame);
        glDispatchCompute(256 / 32, 256 / 32, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(qwio::get_loaded_shader("default shader"));
        qwgl::uniform("screenspace", glm::ortho<float>(0.f, qw::width, qw::height, 0.f, 0.f, qw::width*2));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wrld.color);
        qwgl::uniform("transform", qwgl::rectangle(0, 0, qw::width, qw::height));
        qw::draw_square();

        qw::poll();
        frame++;
        frame %= 1024;
    }
}