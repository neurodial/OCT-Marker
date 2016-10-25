#include "ptreehelper.h"


#include <boost/property_tree/ptree.hpp>
namespace bpt = boost::property_tree;


boost::property_tree::ptree& PTreeHelper::get_put(boost::property_tree::ptree& tree, const std::string& path)
{
	boost::optional<bpt::ptree&> child = tree.get_child_optional(path);
	if(child)
		return *child;
	return tree.add(path, std::string());
}


boost::property_tree::ptree& PTreeHelper::getNodeWithId(boost::property_tree::ptree& tree, const std::string& searchNode, int id)
{
	for(std::pair<const std::string, bpt::ptree>& treePair : tree)
	{
		if(treePair.first != searchNode)
			continue;

		boost::optional<bpt::ptree&> child = treePair.second.get_child_optional("ID");
		if(child)
		{
			if(id == (*child).get_value<int>(id+1))
				return treePair.second;
		}
	}

	bpt::ptree& node = tree.add(searchNode, std::string());
	node.add("ID", id);

	return node;
}

