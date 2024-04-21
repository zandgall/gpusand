#include "quickwork_ui.h"
#include "quickwork_io.h"
#include "quickwork_gl.h"
#include "quickwork.h"
#include "quickwork_au.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
using namespace qwui;

glm::vec2 anchor::getPos(const frame* screen) const 	{
	glm::vec2 offset = screen==nullptr ? glm::vec2(0) : screen->offset;
	glm::vec2 screen_size = screen==nullptr ? glm::vec2(qw::width, qw::height) : screen->size;

	if(snapping & center)
		offset.x += screen_size.x / 2;
	else if(snapping & right)
		offset.x += screen_size.x;
	if(snapping & middle)
		offset.y += screen_size.y / 2;
	else if(snapping & bottom)
		offset.y += screen_size.y;

	return getRelativePos()+offset;
}

glm::vec2 component::getPos(const frame* screen) {
	glm::vec2 offset = anch.getPos(screen);
	glm::vec2 size = getSize(screen);
	
	if(alignment & center)
		offset.x -= size.x / 2;
	else if(alignment & right)
		offset.x -= size.x;
	if(alignment & middle)
		offset.y -= size.y / 2;
	else if(alignment & bottom)
		offset.y -= size.y;

	return offset;
}

glm::vec2 text::getSize(const frame* screen) const {
	glm::vec2 y_span = qw::measure_y_span(content, font, font_size);
	return glm::vec2(qw::measure(content, font, font_size), y_span.x + (size_counts_descent ? y_span.y : 0));
}

void text::render(const frame* screen) {
	glUseProgram(qwio::get_loaded_shader("default shader"));
	glm::vec2 y_span = qw::measure_y_span(content, font, font_size);
	glm::vec4 rect = getScreenRect(screen);
	qw::draw_string(content, font, font_size, rect.x, rect.y + y_span.x);
}

void color_background::render(const frame* screen) {
	glUseProgram(qwio::get_loaded_shader("default shader"));
	qwgl::uniform("textured", false);
	qwgl::uniform("color", color);
	qwgl::uniform("transform", qwgl::rectangle(screen->offset.x, screen->offset.y, screen->size.x, screen->size.y));
	qw::draw_square();
}

void image_background::render(const frame* screen) {
	glUseProgram(qwio::get_loaded_shader("default shader"));
	qwgl::uniform("textured", true);
	glBindTexture(GL_TEXTURE_2D, qwio::get_loaded_texture(texture_name.c_str()));
	qwgl::uniform("transform", qwgl::rectangle(screen->offset.x, screen->offset.y, screen->size.x, screen->size.y));
	qw::draw_square();
}

void interface::tick(const frame* screen) {
	frame n = {top_left.getPos(screen), bottom_right.getPos(screen)};
	n.size -= n.offset;
	for(auto a : components) {
		a.second->tick(&n);
	}
}

glm::vec2 interface::getSize(const frame* screen) const {
	return bottom_right.getPos(screen)-top_left.getPos(screen);
}

void interface::render(const frame* screen) {
	frame n = {top_left.getPos(screen), bottom_right.getPos(screen)};
	n.size -= n.offset;
	for(auto a : components) {
		a.second->render(&n);
	}
}

glm::vec2 mouse_interface::getPressedPos(const frame* screen) {
	frame n = {top_left.getPos(screen), bottom_right.getPos(screen)};
	n.size -= n.offset;
	return n.offset + pressed_pos * n.size;
}

glm::vec2 mouse_interface::getHoveredPos(const frame* screen) {
	frame n = {top_left.getPos(screen), bottom_right.getPos(screen)};
	n.size -= n.offset;
	return n.offset + hovered_pos * n.size;
}

void mouse_interface::tick(const frame* screen) {
	frame n = {top_left.getPos(screen), bottom_right.getPos(screen)};
	// size just holds bottom_right coord
	p_hovered = hovered;
	hovered = (qw::mouse_position.x > n.offset.x && qw::mouse_position.x < n.size.x) 
			&& (qw::mouse_position.y > n.offset.y && qw::mouse_position.y < n.size.y);

	n.size -= n.offset;

	if(hovered) {
		hovered_pos = (qw::mouse_position - n.offset) / n.size;
		if(qw::mouse_left || qw::mouse_middle || qw::mouse_right)
			pressed_pos = hovered_pos;
	}

	for(auto a : components) {
		a.second->tick(&n);
	}
}

constexpr char HEX_VAL(const char in) {
	if(in >= '0' && in <= '9')
		return in - '0';
	else if(in >= 'a' && in <= 'f')
		return in - 'a' + 10;
	else if(in >= 'A' && in <= 'F')
		return in - 'A' + 10;
	return 0;
}

constexpr glm::vec4 HEX_COLOR(const char color[6]) {
	glm::vec4 out = glm::vec4(0);
	out.x = (HEX_VAL(color[0])*16+HEX_VAL(color[1])) / 255.f;
	out.y = (HEX_VAL(color[2])*16+HEX_VAL(color[3])) / 255.f;
	out.z = (HEX_VAL(color[4])*16+HEX_VAL(color[5])) / 255.f;
	out.w = 1;
	return out;
}

// UI_Button
#define UI_TEX_SIZE 30
unsigned int ui_button_shader = 0;
unsigned int ui_texture = 0;
void ui_button::render(const frame* screen) {
	if(ui_button_shader == 0) {
		ui_button_shader = qwio::load_shader("res/shaders/ui button.shader");
		ui_texture = qwio::load_texture("res/textures/ui.png");
		glUseProgram(ui_button_shader);
		qwau::load_sound_as("res/sounds/plunk/0.ogg", "plunk.0");
		qwau::load_sound_as("res/sounds/plunk/1.ogg", "plunk.1");
		qwau::load_sound_as("res/sounds/plunk/2.ogg", "plunk.2");
		qwau::load_sound_as("res/sounds/plunk/3.ogg", "plunk.3");
		qwau::load_sound_as("res/sounds/plunk/4.ogg", "plunk.4");
		qwau::load_sound_as("res/sounds/plunk/5.ogg", "plunk.5");
		qwau::load_sound_as("res/sounds/plunk/6.ogg", "plunk.6");
		qwau::load_sound_as("res/sounds/plunk/7.ogg", "plunk.7");
	}

	glUseProgram(ui_button_shader);
	glBindTexture(GL_TEXTURE_2D, ui_texture);
	// qwgl::uniform("screenspace", glm::ortho<float>(0, qw::width, qw::height, 0, 0, qw::width * 2.f));
	qwgl::uniform("screenspace", glm::ortho<float>(0.f, qw::width, qw::height, 0.f, 0.f, qw::width*2));
	glm::vec2 tl = top_left.getPos(screen), br = bottom_right.getPos(screen);
	qwgl::uniform("transform", qwgl::rectangle(tl.x, tl.y, br.x-tl.x, br.y-tl.y));

	float size = 3.f;
	if(br.x-tl.x < UI_TEX_SIZE * 2 || br.y-tl.y < UI_TEX_SIZE * 2)
		size = 1;
	else if(br.x - tl.x < UI_TEX_SIZE * 3 || br.y-tl.y < UI_TEX_SIZE * 3)
		size = 2;

	qwui::text* __text__ = text();
	float content_length = qw::measure(__text__->content, __text__->font, __text__->font_size);
	float max_length = std::max<float>((br.x - tl.x) * 0.85f, br.x - tl.x - size * 32);
	__text__->font_size *= max_length / content_length;
	glm::vec2 span = qw::measure_y_span(__text__->content, __text__->font, __text__->font_size);
	float max_height = br.y - tl.y - size * 16;
	if(span.x > max_height)
		__text__->font_size *= max_height / (span.x);


	qwgl::uniform("pixel_scale", size);

	if(isReleased()) {
		last_released = glfwGetTime();
	}
	if(isHovered()) {
		last_hovered = glfwGetTime();
		if(!wasHovered())
			alSourcePlay(qwau::get_loaded_sound(sound.c_str()));
	}

	if(isPressed()) {
		qwgl::uniform("line", HEX_COLOR("3D3A55"));
		qwgl::uniform("light", HEX_COLOR("ACBFEA"));
		qwgl::uniform("mid", HEX_COLOR("A0B1D5"));
		qwgl::uniform("dark", HEX_COLOR("708AB2"));
	} else if(glfwGetTime() - last_released < 0.2) {
		qwgl::uniform("line", HEX_COLOR("696980"));
		qwgl::uniform("light", HEX_COLOR("E4F6FF"));
		qwgl::uniform("mid", HEX_COLOR("D3E6F6"));
		qwgl::uniform("dark", HEX_COLOR("ABC2D7"));
	} else if(glfwGetTime() - last_hovered < 0.2 || glfwGetTime() - last_released < 0.4) {
		qwgl::uniform("line", HEX_COLOR("484863"));
		qwgl::uniform("light", HEX_COLOR("D4E6FF"));
		qwgl::uniform("mid", HEX_COLOR("C3D6E6"));
		qwgl::uniform("dark", HEX_COLOR("9BB2C7"));
	} else {
		qwgl::uniform("line", HEX_COLOR("403E57"));
		qwgl::uniform("light", HEX_COLOR("CCD9EE"));
		qwgl::uniform("mid", HEX_COLOR("B5C4DE"));
		qwgl::uniform("dark", HEX_COLOR("8DA1C5"));
	}

	__text__->setRelativePos(0, -8*size);
	qw::draw_square();
	if(!isPressed()) {
		qwgl::uniform("transform", qwgl::rectangle(tl.x-size, tl.y-size, br.x-tl.x, br.y-tl.y));
		qw::draw_square();
		__text__->setRelativePos(-size, -9*size);
		if(glfwGetTime() - last_released < 0.2) {
			qwgl::uniform("transform", qwgl::rectangle(tl.x-2*size, tl.y-2*size, br.x-tl.x, br.y-tl.y));
			qw::draw_square();
			qwgl::uniform("transform", qwgl::rectangle(tl.x-3*size, tl.y-3*size, br.x-tl.x, br.y-tl.y));
			qw::draw_square();
			__text__->setRelativePos(-3*size, -3*size-8*size);
		} else if(glfwGetTime() - last_hovered < 0.1 || glfwGetTime() - last_released < 0.4) {
			qwgl::uniform("transform", qwgl::rectangle(tl.x-2*size, tl.y-2*size, br.x-tl.x, br.y-tl.y));
			qw::draw_square();
			__text__->setRelativePos(-2*size, -2*size-8*size);
		}
	}

	interface::render(screen);
}