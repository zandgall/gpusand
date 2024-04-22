#include "rule.h"
#include <regex>
#include <iterator>
#include <fstream>
#include <iostream>

using namespace rules;

std::vector<element> elements = std::vector<element>();
std::map<std::string, unsigned int> properties = std::map<std::string, unsigned int>();

void rules::loadRuleset(std::string file) {
	std::ifstream in(file);
	if(!in.is_open())
		return;
	std::string line;
	std::regex element_param("(?:element ?\\()*(.*?)[,)]");
	while(std::getline(in, line)) {
		if(line.rfind("element", 0) == 0) {
			std::smatch matches;
			std::regex_search(line, matches, element_param);
			for(int i = 0; matches.size(); i++) {
				std::cout << i << ": " << matches[i] << std::endl;
			}
		}
	}
	in.close();
}