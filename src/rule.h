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
		unsigned int compute_shader;
	public:
		rule();
		~rule();
		void run();
		void setMirror(bool x, bool y) {mirrorX = x; mirrorY = y;}
		void setChance(float chance) {this->chance = chance;}
	};

	extern std::vector<element> elements;
	extern std::map<std::string, unsigned int> properties;
	extern std::vector<rule> ruleset;
	extern void loadRuleset(std::string file);
	extern void applyRuleset();
}