#include "Configuration.h"
#include <iostream>

using namespace std;

Configuration::Configuration(char* file) {

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(file);
	pugi::xml_node root = doc.child("demoElement");
	if (result)
	{
		std::cout << "XML [" << file << "] parsed without errors ] \n";
	}
	else
	{
		std::cout << "XML [" << file << "] parsed with errors, attr value: [" << doc.child("demoElement").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << (file + result.offset) << "]\n\n";
	}

	for (pugi::xml_node node = doc.child("demoElement").first_child(); node; node = node.next_sibling()) {
		cout << node.name() << ": " << node.child_value() << endl;
	}

	m_numProjectors = root.attribute("projectors").as_int();

	m_projectorWidth = root.attribute("width").as_int();

	m_projectorHeight = root.attribute("height").as_int();

	m_xml_display = root.attribute("display").value();

	m_xml_displayID = root.attribute("displayID").as_int();

	m_xml_tracker = root.attribute("tracker").value();

	m_xml_renderer = root.attribute("renderer").value();


}

void Configuration::SetConfigurationVariables(char* file){ return; }

int Configuration::GetDisplayID(){
	return m_xml_displayID;
}

int Configuration::GetNumberOfProjectors() {
	return m_numProjectors;
}

int Configuration::GetWidthProjectors() {
	return m_projectorWidth;
}

int Configuration::GetHeightProjectors() {
	return m_projectorHeight;
}

string  Configuration::GetTrackerType() {
	return m_xml_tracker;
}
string  Configuration::GetDisplayType() {
	return m_xml_display;
}
string  Configuration::GetRendererType() {
	return m_xml_renderer;
}



