#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>

namespace rules {

	extern void loadRuleset(std::string file);

	struct element {
		glm::vec3 color, hsv_variance;
		char key;
	};

	extern std::vector<element> elements;
	extern std::map<std::string, unsigned int> properties;

	class rule {
	private:
		bool mirrorX, mirrorY;
		float chance = 1.0;
		std::map<std::string, std::string> input;
		unsigned int compute_shader;
	public:
		rule();
		~rule();
		void run();
	};

}