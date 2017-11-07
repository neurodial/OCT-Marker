#ifndef PTREEHELPER_H
#define PTREEHELPER_H


#include <boost/property_tree/ptree_fwd.hpp>
#include <string>

namespace PTreeHelper
{
	boost::property_tree::ptree& get_put      (boost::property_tree::ptree& tree, const std::string& path);
	boost::property_tree::ptree& getNodeWithId(boost::property_tree::ptree& tree, const std::string& searchNode, int id);

	void putNotEmpty(boost::property_tree::ptree& tree, const std::string& path, const std::string& value);


	class NodeCreator
	{
		const std::string name;
		boost::property_tree::ptree* node =  nullptr;

		NodeCreator* const parentCreator = nullptr;
		boost::property_tree::ptree*  const parentNode    = nullptr;

		bool createId = false;
		std::size_t id;
	public:
		NodeCreator(const std::string& name, NodeCreator                & parent) : name(name), parentCreator(&parent) {}
		NodeCreator(const std::string& name, boost::property_tree::ptree& parent) : name(name), parentNode   (&parent) {}

		void setId(std::size_t id) { this->id = id; createId = true; }

		boost::property_tree::ptree& getNode();
	};
};

#endif // PTREEHELPER_H
