#pragma once
#ifndef QUICKWORK_UI_H
#define QUICKWORK_UI_H
#include "quickwork.h"
#include <glm/glm.hpp>
#include <string>
#include <map>
namespace qwui {
	enum snap {
		left = 1, center = 2, right = 4,
		top = 8, middle = 16, bottom = 32,

		topLeft = top|left, topCenter = top|center, topRight = top|right,
		middleLeft = middle|left, middleCenter = middle|center, middleRight = middle|right,
		bottomLeft = bottom|left, bottomCenter = bottom|center, bottomRight = bottom|right
	};

	struct frame {
		glm::vec2 offset, size;
	};

	class anchor {
	protected:
		glm::vec2 relative_pos;
		qwui::snap snapping;
	public:
		anchor(): relative_pos(0), snapping(topLeft) {}
		anchor(glm::vec2 r, qwui::snap s = topLeft): relative_pos(r), snapping(s) {}
		glm::vec2 getPos(const frame* screen) const;
		glm::vec2 getRelativePos() const {return relative_pos;}
		void setRelativePos(glm::vec2 pos) {this->relative_pos = pos;}
		void setRelativePos(float x, float y) {this->relative_pos = glm::vec2(x, y);}
		void setSnapping(qwui::snap snapping) {this->snapping = snapping;}
		void set(glm::vec2 pos, qwui::snap snapping) {setRelativePos(pos); setSnapping(snapping);}
		void set(qwui::snap snapping, glm::vec2 pos) {setRelativePos(pos); setSnapping(snapping);}
		void set(float x, float y, qwui::snap snapping) {setRelativePos(x,y); setSnapping(snapping);}
		void set(qwui::snap snapping, float x, float y) {setRelativePos(x,y); setSnapping(snapping);}
		int getSnapping() const {return snapping;}
	};

	class component {
	protected:
		anchor anch;
		qwui::snap alignment;
	public:
		component(): anch(), alignment(topLeft) {}
		glm::vec2 getPos(const frame* screen); // Returns position on screen

		virtual void tick(const frame* screen = nullptr) = 0;
		virtual glm::vec2 getSize(const frame* screen) const = 0; // Returns size of element
		virtual void render(const frame* screen = nullptr) = 0;

		virtual glm::vec4 getScreenRect(const frame* screen) {return glm::vec4(getPos(screen), getSize(screen));} // Returns the position and size of the element on screen
		virtual glm::vec2 getRelativePos() {return anch.getRelativePos();} // Returns pos relative to the anchors
		virtual void setRelativePos(float x, float y) {anch.setRelativePos(glm::vec2(x, y));}
		virtual void setAlignment(qwui::snap a) {alignment = a;} // Sets the anchor for the element. If it were, say, centered, then getPos will offset by half 
		virtual void setSnapping(qwui::snap a) {anch.setSnapping(a);}
	};

	class text : public component {
	private:
		bool size_counts_descent = false;
	public:
		std::string content, font;
		float font_size;

		text(std::string content, std::string font, float font_size): content(content), font(font), font_size(font_size) {}

		void tick(const frame* screen = nullptr) {}

		glm::vec2 getSize(const frame* screen) const;
		void render(const frame* screen = nullptr);
		/**
		 * If this is true, then the result of 'getSize' takes into account the descent of a string of text. Meaning the tails of letters that hang below the baseline, like 'q', 'p', and 'g'.
		**/
		void sizeCountsDescent(bool size_counts_descent) {this->size_counts_descent = size_counts_descent;}
		bool sizeCountsDescent() {return size_counts_descent;}
	};

	class color_background : public component {
	public:
		glm::vec4 color;
		color_background(glm::vec4 color = glm::vec4(1)): color(color) {}

		void tick(const frame* screen = nullptr) {}

		glm::vec2 getSize(const frame* screen) const {
			return screen->size;
		}
		void render(const frame* screen = nullptr);
	};

	class image_background : public component {
	public:
		std::string texture_name; // quickwork_io.h
		image_background(std::string texture_name = ""): texture_name(texture_name) {}

		void tick(const frame* screen = nullptr) {}

		glm::vec2 getSize(const frame* screen) const {
			return screen->size;
		}
		void render(const frame* screen = nullptr);
	};

	class interface : public component {
	protected:
		anchor top_left, bottom_right;
	public:
		interface() : component(), top_left(), bottom_right(glm::vec2(0), snap::bottomRight) {}
		std::map<std::string, component*> components = std::map<std::string, component*>();
		virtual void tick(const frame* screen = nullptr);
		virtual glm::vec2 getSize(const frame* screen) const;
		virtual void render(const frame* screen = nullptr);
		anchor& topLeft() {return top_left;}
		anchor& bottomRight() {return bottom_right;}
	};

	class mouse_interface : public interface {
	protected:
		bool hovered, p_hovered;
		glm::vec2 pressed_pos, hovered_pos;
	public:
		mouse_interface(): interface() {}
		bool isHovered() {return hovered;}
		bool wasHovered() {return p_hovered;}
		bool isPressed() {return hovered && (qw::mouse_left || qw::mouse_middle || qw::mouse_right);}
		bool isLeftPressed() {return hovered && qw::mouse_left;}
		bool isMiddlePressed() {return hovered && qw::mouse_middle;}
		bool isRightPressed() {return hovered && qw::mouse_right;}
		bool isReleased() {return hovered && ((qw::p_mouse_left && !qw::mouse_left) || (qw::p_mouse_right && !qw::mouse_right) || (qw::p_mouse_middle && !qw::mouse_middle));}
		bool isLeftReleased() {return hovered && qw::p_mouse_left && !qw::mouse_left;}
		bool isMiddleReleased() {return hovered && qw::p_mouse_middle && !qw::mouse_middle;}
		bool isRightReleased() {return hovered && qw::p_mouse_right && !qw::mouse_right;}
		glm::vec2 getScroll() {return hovered ? glm::vec2(0) : qw::scroll;}
		// x,y => [0, 1]
		glm::vec2 getPressedRelativePos() {return pressed_pos;}
		// x,y => [0, 1]
		glm::vec2 getHoveredRelativePos() {return hovered_pos;}
		glm::vec2 getPressedPos(const frame* screen = nullptr);
		glm::vec2 getHoveredPos(const frame* screen = nullptr);
		virtual void tick(const frame* screen = nullptr);
	};


	// Specific implementations

	class ui_button : public mouse_interface {
	private:
		double last_released = 0, last_hovered = 0;
		std::string sound;
	public:
		ui_button(std::string text = "Button", std::string sound = "0"): mouse_interface(), sound(sound) {
			components["__text__"] = new qwui::text(text, "basicbit", 96);
			components["__text__"]->setAlignment(qwui::snap::bottomCenter);
			components["__text__"]->setSnapping(qwui::snap::bottomCenter);
		}
		qwui::text* text() {return static_cast<qwui::text*>(components["__text__"]);}
		void render(const frame* screen = nullptr);
	};

}
#endif