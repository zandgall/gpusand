#include "rule.h"
#include <regex>
#include <iterator>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "qw/quickwork.h"
#include "qw/quickwork_io.h"
#include "qw/quickwork_gl.h"

using namespace rules;

std::vector<element> rules::elements = std::vector<element>();
std::map<std::string, unsigned int> rules::properties = std::map<std::string, unsigned int>();
std::map<char, unsigned int> rules::element_keymap = std::map<char, unsigned int>();
std::vector<rule> rules::ruleset = std::vector<rule>();

std::map parsed_tags = std::map<std::string, std::vector<unsigned int>>();

std::regex item(R"((.*?)\s*\{\s*((?:\s.*?)*)\})");

std::regex rule_flags(R"(rule(.*?)\((.*)\))");
std::regex rule_chance(R"((\d+)%)");
std::regex rule_param(R"((.):\s*(.*?)(?:,|$))");
std::regex rule_line(R"((\S+))");

std::string SHADER_HEADER = 
R"(#version 460 compatibility

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform coherent volatile image2D world_color;
layout (r32ui, binding = 1) uniform coherent volatile uimage2D world_dead;

layout (location = 2) uniform int frame;
layout (rgba32f, location = 3) uniform readonly image1D noise;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
})";

void parseRule(std::smatch groups) {
	int index = rules::ruleset.size();
	rule out;
	bool mirrorX, mirrorY;
	std::string mirror_x_str, mirror_y_str;
	std::smatch flags;
	std::string contents = groups[1].str();
	std::regex_search(contents, flags, rule_flags);
	mirrorX = (flags[1].str().find("x")!=std::string::npos);
	mirror_x_str = mirrorX ? "xmir * " : ""; 
	mirrorY = (flags[1].str().find("y")!=std::string::npos);
	mirror_y_str = mirrorY ? "ymir * " : ""; 

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
		if(std::find(out.used_identities.begin(), out.used_identities.end(), identity)==out.used_identities.end())
			out.used_identities.push_back(identity);
	}

	contents = groups[2].str();
	std::regex_search(contents, flags, rule_line);
	int rule_width = flags[1].length(), rule_height = 0, n = 0;

	std::map match_positions = std::map<char, glm::ivec2>(), replace_positions = std::map<char, glm::ivec2>();
	std::map match_pos_str = std::map<char, std::string>(), replace_pos_str = std::map<char, std::string>();
	while(flags.size()>0) {
		if(flags[1].str()=="=>") {
			rule_height = n;
			break;
		}

		std::string identifiers = flags[1].str();
		for(int i = 0; i < identifiers.length(); i++)
			if(identifiers[i] != '.') {
				match_positions[identifiers[i]] = glm::ivec2(i, n);
				match_pos_str[identifiers[i]] = (i==0 && n==0) ? "" : 
				("+ ivec2(" + (i==0 ? "0" : (mirror_x_str + std::to_string(i))) + ", "
							+ (n==0 ? "0" : (mirror_y_str + std::to_string(n))) + ")");
			}
		
		contents=contents.substr(flags.length()+flags.position());
		std::regex_search(contents, flags, rule_line);
		n++;
	}
	n = 0;
	contents=contents.substr(flags.length()+flags.position());
	std::regex_search(contents, flags, rule_line);
	while(flags.size()>0) {
		std::string identifiers = flags[1].str();
		for(int i = 0; i < identifiers.length(); i++)
			if(identifiers[i]!='.') {
				replace_positions[identifiers[i]] = glm::ivec2(i,n);
				replace_pos_str[identifiers[i]] = (i==0 && n==0) ? "" : 
				("+ ivec2(" + (i==0 ? "0" : (mirror_x_str + std::to_string(i))) + ", " 
							+ (n==0 ? "0" : (mirror_y_str + std::to_string(n))) + ")");
			}

		contents=contents.substr(flags.length()+flags.position());
		std::regex_search(contents, flags, rule_line);
		n++;
	}

	std::stringstream shader_code;
	shader_code << SHADER_HEADER << "\n";
	n = 2;
	for(int i = 0; i < out.used_identities.size(); i++) {
		shader_code << "layout (r32ui, location = "<< (i+4) << ") uniform readonly uimage1D " << out.used_identities[i] << ";\n";
	}
	shader_code << "void main() {\n\tivec2 coord = ivec2(gl_GlobalInvocationID.xy);";
	shader_code << "\n\tivec2 bounds = imageSize(world_dead), c = coord;";
	if(mirrorX) shader_code << "\n\tint xmir = imageLoad(noise, (frame + c.x + c.y*bounds.x)%1024).r < 0.5 ? 1 : -1;";
	if(mirrorY) shader_code << "\n\tint ymir = imageLoad(noise, (frame + c.y + c.x*bounds.y)%1024).r < 0.5 ? 1 : -1;";
	shader_code << "\n";
	for(auto a = match_positions.begin(); a!=match_positions.end(); a++) {
		shader_code << "\tc = coord" << match_pos_str[a->first] << ";\n";
		shader_code << "\tuint _" << a->first << " = imageLoad(world_dead, c).r;\n";
		shader_code << "\tif(c.x >= bounds.x || c.y >= bounds.y || !match(_" << a->first << ", " << parameters[a->first] << "))\n\t\treturn;\n";
		// shader_code << "\tuint _" << a->first << ""
	}
	for(auto a = replace_positions.begin(); a!=replace_positions.end(); a++)
		if(a->second!=match_positions[a->first])
			shader_code << "\timageStore(world_dead, coord" << replace_pos_str[a->first] << ", uvec4(_" << a->first << ", 0, 0, 0));\n";
	for(auto a = replace_positions.begin(); a!=replace_positions.end(); a++)
		if(a->second!=match_positions[a->first])
			shader_code << "\tvec4 c"<<a->first << " = imageLoad(world_color, coord" << match_pos_str[a->first] << ");\n";
	for(auto a = replace_positions.begin(); a!=replace_positions.end(); a++)
		if(a->second!=match_positions[a->first])
			shader_code << "\timageStore(world_color, coord" << replace_pos_str[a->first] << ", c" << a->first << ");\n";

	shader_code << "\tmemoryBarrierShared();\n}\n";

	std::cout << shader_code.str(); 

	out.compute_shader = glCreateProgram();
	unsigned int cs = qwio::compileShader(GL_COMPUTE_SHADER, shader_code.str());
	glAttachShader(out.compute_shader, cs);
	glLinkProgram(out.compute_shader);
	glValidateProgram(out.compute_shader);
	glDeleteShader(cs);
	glUseProgram(out.compute_shader);
	qwgl::uniform("frame", 0);
	qwgl::uniform("noise", 3);
	for(int i = 0; i < out.used_identities.size(); i++)
		qwgl::uniform(out.used_identities[i].c_str(), i+4);
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
			char k = (char)toupper(props[2].str()[0]);
			out.key = k;
			element_keymap[k] = index;
			std::cout << "mapping \'" << k << "' (" << (int)k << ") to " << index << std::endl;
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

void rules::applyRuleset(unsigned int noise_texture) {
	for(auto a = parsed_tags.begin(); a!=parsed_tags.end(); a++) {
		properties[a->first] = 0;
		glGenTextures(1, &properties[a->first]);
		glBindTexture(GL_TEXTURE_1D, properties[a->first]);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_R32UI, a->second.size(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &a->second[0]);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	// for(auto a : ruleset) {
	// 	glUseProgram(a.compute_shader);
	// }
}

rule::rule() {
	mirrorX = false;
	mirrorY = false;
	// input = std::map<std::string, std::string>();
	compute_shader = 0;
}

rule::~rule() {

}

void rule::run(unsigned int noise_texture) {
	glUseProgram(compute_shader);
	for(int i = 0; i < used_identities.size(); i++)
		glBindImageTexture(i+4, properties[used_identities[i]], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32UI);
	qwgl::uniform("frame", ran++);

	glBindImageTexture(3, noise_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glDispatchCompute(256 / 8, 256 / 8, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    ran %= 1024;
}