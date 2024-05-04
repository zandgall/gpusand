#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>

namespace rules {

	struct element {
		glm::vec3 color, hsv_variance;
		char key;
		element() {
			color = glm::vec3(0);
			hsv_variance = glm::vec3(0);
			key = 0;
		}
	};

	class rule {
	private:
		bool mirrorX, mirrorY;
		float chance = 1.0;
		int ran = 0;
	public:
		unsigned int compute_shader;
		std::vector<std::string> used_identities = std::vector<std::string>();
		rule();
		~rule();
		void run(unsigned int noise_texture, unsigned int buffer);
		void setMirror(bool x, bool y) {mirrorX = x; mirrorY = y;}
		void setChance(float chance) {this->chance = chance;}
	};

	extern std::vector<element> elements;
	extern std::map<std::string, unsigned int> properties;
	extern std::map<char, unsigned int> element_keymap;
	extern std::vector<rule> ruleset;
	extern void loadRuleset(std::string file);
	extern void applyRuleset(unsigned int noise);
}