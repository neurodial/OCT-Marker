/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ptreehelper.h"


#include <boost/property_tree/ptree.hpp>
namespace bpt = boost::property_tree;

namespace PTreeHelper
{

	boost::property_tree::ptree& get_put(boost::property_tree::ptree& tree, const std::string& path)
	{
		boost::optional<bpt::ptree&> child = tree.get_child_optional(path);
		if(child)
			return *child;
		return tree.add(path, std::string());
	}


	boost::property_tree::ptree& getNodeWithId(boost::property_tree::ptree& tree, const std::string& searchNode, int id)
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

	void putNotEmpty(boost::property_tree::ptree& tree, const std::string& path, const std::string& value)
	{
		if(!value.empty())
			tree.put(path, value);
	}



	boost::property_tree::ptree& NodeCreator::getNode()
	{
		if(!node)
		{
			bpt::ptree* parent = parentNode;
			if(!parent)
				parent = &(parentCreator->getNode());
			node = &(parent->add(name, ""));

			if(createId)
				node->add("ID", id);
		}
		return *node;
	}

}
