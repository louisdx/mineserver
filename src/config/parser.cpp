/*
Copyright (c) 2011, The Mineserver Project
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the The Mineserver Project nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string>
#include <map>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <stack>
#include <boost/shared_ptr.hpp>

#include "scanner.h"
//#include "lexer.h"
#include "parser.h"
#include "node.h"

#include "../tools.h"

// local util functions
bool get_token( ConfigScanner* m_scanner,
					  std::stack< boost::shared_ptr <std::pair<int, std::string> > > *m_tokenStack,
					  int* type, 
					  std::string* data);

ConfigParser::ConfigParser()
{
	m_includes = 0;
}

bool ConfigParser::parse(const std::string& file, ConfigNode* ptr)
{
	struct stat st;
	std::ifstream ifs;

	if ((stat(file.c_str(), &st) != 0) || (st.st_size >= MAX_FILESIZE))
	{
		std::cerr << "Couldn't stat file: " << file << "\n";
		return false;
	}

	ifs.open(file.c_str(), std::ios_base::binary);
	if (ifs.fail())
	{
		std::cerr << "Couldn't open file: " << file << "\n";
		return false;
	}

	bool ret = false;
	ret = parse(ifs, ptr);

	ifs.close();
	return ret;
}

bool ConfigParser::parse(const std::istream& data, ConfigNode* ptr)
{
	std::stack< boost::shared_ptr <std::pair<int, std::string> > > m_tokenStack;
	ConfigScanner scanner;
	ConfigNode* root = ptr;

	// that's ugly!
	std::stringstream ss;
	ss << data.rdbuf();

	if (!scanner.read(ss.str()))
	{
		std::cerr << "Couldn't read data!\n";
		return false;
	}

	int token_type;
	std::string token_data;
	std::string token_label;
	std::deque<ConfigNode*> nodeStack;
	ConfigNode* currentNode = root;
	nodeStack.push_back(currentNode);
	while (get_token(&scanner,&m_tokenStack,&token_type, &token_data))
	{
		if (!token_type)
		{
			std::cerr << "Unrecognised data!\n";
			return false;
		}

		// Include other files only if we're in the root node
		if ((token_type == CONFIG_TOKEN_ENTITY) && (token_data == "include") && (currentNode == root))
		{
			int tmp_type;
			std::string tmp_data;

			get_token(&scanner,&m_tokenStack,&tmp_type, &tmp_data);
			if (tmp_type == CONFIG_TOKEN_STRING)
			{
				if (m_includes >= MAX_INCLUDES)
				{
					std::cerr << "reached maximum number of include directives: " << m_includes << "\n";
					return false;
				}

				// allow only filename without path
				if ((tmp_data.find('/')  != std::string::npos)
					|| (tmp_data.find('\\') != std::string::npos))
				{
					std::cerr << "include directive accepts only filename: " << tmp_data << "\n";
					return false;
				}

				// prepend home path
				const std::string var  = "system.path.home";
				ConfigNode*       node = root->get(var, false);
				std::string       home;
				if (!node || (home = node->sData()).empty())
				{
					std::cerr << "include directive is not allowed before: " << var << "\n";
					return false;
				}

				tmp_data = pathExpandUser(home) + '/' + tmp_data;
				if (!parse(tmp_data, root))
				{
					return false;
				}
				m_includes++;

				continue;
			}
			else
			{
				boost::shared_ptr< std::pair<int, std::string> > newToken(
					new std::pair<int, std::string>(tmp_type,tmp_data));
				m_tokenStack.push(newToken);
			}
		}

		if ((token_type == CONFIG_TOKEN_ENTITY) || (token_type == CONFIG_TOKEN_LABEL))
		{
			token_label.assign(token_data);
		}

		if (token_type == CONFIG_TOKEN_OPERATOR_ASSIGN)
		{
			if (currentNode != root)
			{
				currentNode->clear();
			}
		}

		if (token_type == CONFIG_TOKEN_BOOLEAN)
		{
			ConfigNode* newNode = (token_label.size() && currentNode->has(token_label)) ? currentNode->get(token_label) : new ConfigNode;

			newNode->setData(token_data == "true");

			if (token_label.size())
			{
				currentNode->set(token_label, newNode, true);
				token_label.clear();
			}
			else
			{
				currentNode->add(newNode);
			}
		}

		if (token_type == CONFIG_TOKEN_STRING)
		{
			ConfigNode* newNode = (token_label.size() && currentNode->has(token_label)) ? currentNode->get(token_label) : new ConfigNode;

			newNode->setData(token_data);

			if (token_label.size())
			{
				currentNode->set(token_label, newNode, true);
				token_label.clear();
			}
			else
			{
				currentNode->add(newNode);
			}
		}

		if (token_type == CONFIG_TOKEN_NUMBER)
		{
			ConfigNode* newNode = (token_label.size() && currentNode->has(token_label)) ? currentNode->get(token_label) : new ConfigNode;

			newNode->setData((double)::atof(token_data.c_str()));

			if (token_label.size())
			{
				currentNode->set(token_label, newNode, true);
				token_label.clear();
			}
			else
			{
				currentNode->add(newNode);
			}
		}

		if (token_type == CONFIG_TOKEN_LIST_OPEN)
		{
			ConfigNode* newNode = (token_label.size() && currentNode->has(token_label)) ? currentNode->get(token_label) : new ConfigNode;

			newNode->setType(CONFIG_NODE_LIST);

			if (token_label.size())
			{
				currentNode->set(token_label, newNode, true);

				newNode = currentNode->get(token_label, true);

				token_label.clear();
			}
			else
			{
				currentNode->add(newNode);
			}

			nodeStack.push_back(currentNode);
			currentNode = newNode;
		}

		if (token_type == CONFIG_TOKEN_LIST_CLOSE)
		{
			currentNode = nodeStack.back();
			nodeStack.pop_back();
		}
	}

	return true;
}

bool get_token( ConfigScanner* m_scanner,
							std::stack< boost::shared_ptr <std::pair<int, std::string> > > *m_tokenStack,
							int* type, 
							std::string* data)
{
	*type = 0;
	data->clear();

	if (m_tokenStack->size())
	{
		boost::shared_ptr< std::pair<int, std::string> > top = m_tokenStack->top();
		*type = top->first;
		data->assign(top->second);
		m_tokenStack->pop();
		return true;
	}

	char buf;
	buf = m_scanner->get();

	// Skip past spaces and newlines
	while ((buf == ' ') || (buf == '\n') || (buf == '\r') || (buf == '\t'))
	{
		m_scanner->move(1);
		buf = m_scanner->get();
	}

	// Nothing left to parse! Bail out!
	if (m_scanner->left() <= 0)
	{
		return false;
	}

	// Entity
	// Always starts with a letter, can contain only letters, numbers, periods and underscores
	if (((buf >= 'a') && (buf <= 'z')) || ((buf >= 'A') && (buf <= 'Z')))
	{
		while (((buf >= 'a') && (buf <= 'z')) || ((buf >= 'A') && (buf <= 'Z')) || ((buf >= '0') && (buf <= '9')) || (buf == '.') || (buf == '_'))
		{
			data->append(&buf, 1);
			m_scanner->move(1);
			buf = m_scanner->get();
		}

		// Check for boolean literal is true and false
		// This prevents us using true and false as identifiers.
		if (*data == "true" || *data == "false")
		{
			*type = CONFIG_TOKEN_BOOLEAN;
		}
		else
		{
			*type = CONFIG_TOKEN_ENTITY;
		}

		return true;
	}
	// Number
	// Always starts with a digit, may contain at most one decimal point
	else if ((buf >= '0') && (buf <= '9'))
	{
		bool found = false;

		while (((buf >= '0') && (buf <= '9')) || ((buf == '.') && (found == false)) || (buf == '_'))
		{
			if (buf == '.')
			{
				found = true;
			}

			if (buf != '_')
			{
				data->append(&buf, 1);
			}

			m_scanner->move(1);
			buf = m_scanner->get();
		}

		*type = CONFIG_TOKEN_NUMBER;
		return true;
	}
	// Assignment operator
	else if (buf == '=')
	{
		*type = CONFIG_TOKEN_OPERATOR_ASSIGN;
		m_scanner->move(1);
		return true;
	}
	// Addition operator
	else if ((buf == '+') && (m_scanner->at(m_scanner->pos() + 1) == '='))
	{
		*type = CONFIG_TOKEN_OPERATOR_APPEND;
		m_scanner->move(2);
		return true;
	}
	// Quoted string
	else if ((buf == '"') || (buf == '\''))
	{
		// Save the type of quote
		int quote = buf;

		// Move forward one character
		m_scanner->move(1);

		// Parse the string and any escape characters
		char temp;
		while (m_scanner->left() > 0)
		{
			// Avoid a couple of get() calls
			temp = m_scanner->get();

			// We've found the end of the string
			if (temp == quote)
			{
				// Time to stop parsing!
				break;
			}

			// This is used to escape other characters or itself
			if (temp == '\\')
			{
				// Skip past the slash
				m_scanner->move(1);

				// Get the next character regardless of what it is
				temp = m_scanner->get();

				// Control characters (more can/will be added)
				switch (temp)
				{
					// New line
				case 'n':
					temp = '\n';
					break;
				}
			}

			// Add the character to the data string
			data->append(&temp, 1);

			// Move forward one
			m_scanner->move(1);
		}

		// Skip past the ending quote
		m_scanner->move(1);

		// Record where the scanner was at before
		int old_pos = m_scanner->pos();
		// Skip past any whitespace
		buf = m_scanner->get();
		while ((buf == ' ') || (buf == '\n') || (buf == '\r') || (buf == '\t'))
		{
			m_scanner->move(1);
			buf = m_scanner->get();
		}

		// A quoted string followed by a colon is a label
		if (buf == ':')
		{
			*type = CONFIG_TOKEN_LABEL;
			m_scanner->move(1);
		}
		else
		{
			*type = CONFIG_TOKEN_STRING;
			m_scanner->move(0 - (m_scanner->pos() - old_pos));
		}

		return true;
	}
	// End of statement
	else if (buf == ';')
	{
		*type = CONFIG_TOKEN_TERMINATOR;
		m_scanner->move(1);
		return true;
	}
	// Delimiter for naming list items
	else if (buf == ':')
	{
		*type = CONFIG_TOKEN_LIST_DELIMITER;
		m_scanner->move(1);
		return true;
	}
	// Delimiter for separating list items
	else if (buf == ',')
	{
		*type = CONFIG_TOKEN_LIST_DELIMITER;
		m_scanner->move(1);
		return true;
	}
	// Start of list
	else if (buf == '(')
	{
		*type = CONFIG_TOKEN_LIST_OPEN;
		m_scanner->move(1);
		return true;
	}
	// End of list
	else if (buf == ')')
	{
		*type = CONFIG_TOKEN_LIST_CLOSE;
		m_scanner->move(1);
		return true;
	}
	// Skip commented characters
	else if (buf == '#')
	{
		while (buf != '\n')
		{
			m_scanner->move(1);
			buf = m_scanner->get();
		}

		return get_token(m_scanner,m_tokenStack,type, data);
	}
	// Unknown data, throw warnings everywhere!
	else
	{
		*type = 0;
		m_scanner->move(1);
		return true;
	}
}
