/* Initialization Variables, set up in Main function. Determine constants used throughout the project*/
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include <string.h>

#include "pugixml.hpp"
using namespace std;

class Configuration {

public:

	Configuration(char* file);
	~Configuration();
	void SetConfigurationVariables(char* file);
	//void SetConfigurationVariables();
	string GetTrackerType();

	string GetDisplayType();
	int GetDisplayID();

	//If it's a cinderApp we will be using OpenGL for this
	string GetRendererType(); 
	

	int GetNumberOfProjectors();
	int GetWidthProjectors();
	int GetHeightProjectors();
	int m_numProjectors;
	const pugi::char_t* m_xml_tracker;
	const pugi::char_t* m_xml_input;
	const pugi::char_t* m_xml_renderer;
	int m_projectorWidth;
	int m_projectorHeight;
	const pugi::char_t* m_xml_display;
	int m_xml_displayID;
	const pugi::char_t* m_xml_demo;



};

#endif



