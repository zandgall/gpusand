#include "rule.h"
#include <regex>
#include <iterator>
#include <fstream>
#include <iostream>

using namespace rules;

std::vector<element> rules::elements = std::vector<element>();
std::map<std::string, unsigned int> rules::properties = std::map<std::string, unsigned int>();
std::vector<rule> rules::ruleset = std::vector<rule>();

std::map parsed_tags = std::map<std::string, std::vector<unsigned int>>();
std::vector rules_match = std::vector<std::vector<std::vector<std::string>>>(), rules_replace = std::vector<std::vector<std::vector<std::string>>>();

std::regex item(R"((.*?)\s*\{\s*((?:\s.*?)*)\})");

std::regex rule_flags(R"(rule(.*?)\((.*)\))");
std::regex rule_chance(R"((\d+)%)");
std::regex rule_param(R"((.):\s*(.*?)(?:,|$))");
std::regex rule_line(R"((\S+))");

std::string SHADER_HEADER = 
R"(#version 460 compatibility

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform image2D world_color;
layout (r32ui, binding = 1) uniform uimage2D world_dead;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
})";

void parseRule(std::smatch groups) {
	int index = rules::ruleset.size();
	rule out;
	std::smatch flags;
	std::string contents = groups[1].str();
	std::regex_search(contents, flags, rule_flags);
	out.setMirror((flags[1].str().find("x")!=std::string::npos), (flags[1].str().find("y")!=std::string::npos));
	if(flags[1].str().find("%")!=std::string::npos) {
		std::smatch chance;
		contents = flags[1].str();
		std::regex_search(contents, chance, rule_chance);
		out.setChance(std::stoi(chance[1].str()) / 100.f);
	}
	contents = flags[2].str();
	std::map<char, std::string> parameters = std::map<char, std::string>();
	for(std::regex_search(contents, flags, rule_param); flags.size()>0; contents=contents.substr(flags.length()+flags.position()), std::regex_search(contents, flags, rule_param)) {
		std::string identity = flags[2].str();
		if(parsed_tags.find(identity)==parsed_tags.end())
			parsed_tags[identity] = std::vector<unsigned int>();
		parameters[flags[1].str()[0]] = identity;
	}

	contents = groups[2].str();
	std::regex_search(contents, flags, rule_line);
	int rule_width = flags[1].length(), rule_height = 0, n;

	rules_match.push_back(std::vector<std::vector<std::string>>());
	rules_replace.push_back(std::vector<std::vector<std::string>>());
	std::vector<std::vector<std::vector<std::string>>> *block = &rules_match;
	while(flags.size()>0) {
		
		int block_line = block->at(index).size();
		
		block->at(index).push_back(std::vector<std::string>());
		if(flags[1].str()=="=>") {
			rule_height = n;
			block = &rules_replace;
			contents=contents.substr(flags.length()+flags.position());
			std::regex_search(contents, flags, rule_line);
			continue;
		}

		std::string identifiers = flags[1].str();

		for(int i = 0; i < identifiers.length(); i++)
			block->at(index)[block_line].push_back(parameters[identifiers[i]]);

		contents=contents.substr(flags.length()+flags.position());
		std::regex_search(contents, flags, rule_line);
		n++;
	}
	rules::ruleset.push_back(out);
}

std::regex element_property(R"(\s*(.*):\s*(.*))");
std::regex hex_color(R"(#([0-9a-fA-F]{6}))");
std::regex variance(R"((\d+)%\s?(\d+)%\s?(\d+)%)");

void parseElement(std::smatch elements) {
	int index = rules::elements.size();
	element out;
	std::vector tags = std::vector<std::string>();
	tags.push_back(elements[1].str());
	std::smatch props;
	std::string contents = elements[2].str();
	for(std::regex_search(contents, props, element_property); props.size()>0; contents=contents.substr(props.length()+props.position()), std::regex_search(contents, props, element_property)) {
		if(props[1].str()=="tags") {
			std::string tag_list = props[2].str();
			unsigned long long i;
			while((i = tag_list.find(","))!=std::string::npos) {
				std::cout << "tag: " << tag_list.substr(0, i) << std::endl;
				tags.push_back(tag_list.substr(0, i));
				if(tag_list[++i]==' ')
					i++;
				tag_list=tag_list.substr(i);
			}
			while(tag_list[tag_list.size()-1]==' '||tag_list[tag_list.size()-1]=='\t'||tag_list[tag_list.size()-1]=='\n')
				tag_list = tag_list.substr(tag_list.size()-1);
			std::cout << "tag: \"" << tag_list << "\"" << std::endl;
			tags.push_back(tag_list);
		} else if(props[1].str()=="color") {
			std::smatch hex;
			std::string hex_string = props[2].str();
			std::regex_search(hex_string, hex, hex_color);
			int color = std::stoi(hex[1].str(), nullptr, 16);
			out.color = glm::vec3(((color & 0xff0000) >> 16) / 255.f, ((color & 0x00ff00) >> 8) / 255.f, (color & 0x0000ff) / 255.f);
			std::cout << "Color: " <<out.color.x << ", " << out.color.y << ", " << out.color.z << std::endl;
		} else if(props[1].str()=="variance") {
			std::smatch var;
			std::string var_string = props[2].str();
			std::regex_search(var_string, var, variance);
			int h = std::stoi(var[1].str());
			int s = std::stoi(var[2].str());
			int v = std::stoi(var[3].str());
			out.hsv_variance = glm::vec3(h / 100.f, s / 100.f, v / 100.f);
			std::cout << "Variance: " << out.hsv_variance.x << ", " << out.hsv_variance.y << ", " << out.hsv_variance.z << std::endl;
		} else if(props[1].str()=="key") {
			out.key = props[2].str()[0];
		}
	}
	for(std::string i: tags) {
		if(parsed_tags.find(i)==parsed_tags.end())
			parsed_tags[i] = std::vector<unsigned int>();
		parsed_tags[i].push_back(index);
	}
	rules::elements.push_back(out);
}

void rules::loadRuleset(std::string file) {
	std::ifstream in(file);
	if(!in.is_open())
		return;
	std::stringstream ruleset_contents;
	std::string line;
	int lineNum = 0;
	// bool recording = false;
	while(std::getline(in, line)) {
		lineNum++;
		// if(line.find("{") !=std::string::npos) recording = true;
		// if(recording) 
			ruleset_contents << line << "\n";
		// if(line.find("}")!=std::string::npos) recording = false;
	}
	std::string contents = ruleset_contents.str();
	// std::smatch matches;
	// std::regex_search(contents, matches, item);
	// std::cout << matches.length() << std::endl;
	// for(int i = 0; i < matches.length(); i++) {
	// 	std::cout << matches[i].str() << std::endl; 
	// }
	std::smatch matches;
	for(std::regex_search(contents, matches, item); matches.size() > 0; contents=contents.substr(matches.length()+matches.position()), std::regex_search(contents, matches, item)) {
		// std::cout << matches[1].str();
		// std::cout << matches[2].str()<< std::endl;
		if(matches[1].str().find("rule")!=std::string::npos)
			parseRule(matches);
		else {
			parseElement(matches);
		}
	}
	
	in.close();
}

void rules::applyRuleset() {

}

rule::rule() {
	mirrorX = false;
	mirrorY = false;
	// input = std::map<std::string, std::string>();
	compute_shader = 0;
}

rule::~rule() {

}

void rule::run() {

}