﻿
#include "ServerApplicationsTree.h"

namespace HttpServer
{
	ServerApplicationsTree::ServerApplicationsTree(): app_sets(nullptr)
	{

	}

	ServerApplicationsTree::~ServerApplicationsTree()
	{
		clear();
	}

	void ServerApplicationsTree::addApplication(const std::string &name, ServerApplicationSettings *sets)
	{
		std::vector<std::string> name_parts;

		size_t delimiter = name.find('.');

		if (std::string::npos != delimiter)
		{
			size_t cur_pos = 0;

			while (std::string::npos != delimiter)
			{
				std::string part = name.substr(cur_pos, delimiter - cur_pos);

				if ("" == part)
				{
					part = "*";
				}

				name_parts.emplace_back(std::move(part) );
				cur_pos = delimiter + 1;
				delimiter = name.find('.', cur_pos);
			}

			// Emplace last part
			std::string part = name.substr(cur_pos);
			name_parts.emplace_back(std::move(part) );
		}
		else
		{
			name_parts.emplace_back(name);
		}

		addApplication(name_parts, sets);
	}

	void ServerApplicationsTree::addApplication(std::vector<std::string> &nameParts, ServerApplicationSettings *sets)
	{
		if (nameParts.empty() )
		{
			app_sets = sets;
		}
		else
		{
			std::string &part = nameParts.back();

			auto it = list.find(part);

			ServerApplicationsTree *sub;

			if (list.cend() != it)
			{
				sub = it->second;
			}
			else
			{
				sub = new ServerApplicationsTree();
				list.emplace(std::move(part), sub);
			}

			nameParts.pop_back();

			sub->addApplication(nameParts, sets);
		}
	}

	const ServerApplicationSettings *ServerApplicationsTree::find(const std::string &name) const
	{
		std::vector<std::string> name_parts;

		size_t delimiter = name.find('.');

		if (std::string::npos != delimiter)
		{
			size_t cur_pos = 0;

			while (std::string::npos != delimiter)
			{
				std::string part = name.substr(cur_pos, delimiter - cur_pos);
				name_parts.emplace_back(std::move(part) );
				cur_pos = delimiter + 1;
				delimiter = name.find('.', cur_pos);
			}

			std::string part = name.substr(cur_pos);
			name_parts.emplace_back(std::move(part) );
		}
		else
		{
			name_parts.emplace_back(name);
		}

		return find(name_parts);
	}

	const ServerApplicationSettings *ServerApplicationsTree::find(std::vector<std::string> &nameParts) const
	{
		if (nameParts.empty() )
		{
			return app_sets;
		}
		else
		{
			const std::string part = std::move(nameParts.back() );

			nameParts.pop_back();

			auto it = list.find(part);

            if (list.cend() == it)
			{
				it = list.find("*");

				if (list.end() != it)
				{
					return app_sets;
				}
			}
			else
			{
				return it->second->find(nameParts);
			}

			return nullptr;
		}
	}

	void ServerApplicationsTree::collectApplicationSettings(std::unordered_set<ServerApplicationSettings *> &set) const
	{
		for (auto &node : list)
		{
			const ServerApplicationsTree *tree = node.second;

			if (nullptr != tree->app_sets)
			{
				set.emplace(tree->app_sets);
			}

			tree->collectApplicationSettings(set);
		}
	}

	void ServerApplicationsTree::clear()
	{
		if (false == list.empty() )
		{
			for (auto &it : list)
			{
				delete it.second;
			}

			list.clear();
		}
	}
};