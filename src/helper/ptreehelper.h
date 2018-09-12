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
