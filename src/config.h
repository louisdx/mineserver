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

#ifndef _CONFIG_H
#define _CONFIG_H

#include <string>
#include <list>

#include <stdint.h>

#include "config/node.h"

#include <boost/scoped_ptr.hpp>

class ConfigParser;

class Config
{
public:
	Config();
	~Config();

	bool load(const std::string& file);
	bool load(const std::istream& data);
	void dump();

	int iData(const std::string& name);
	int64_t lData(const std::string& name);
	float fData(const std::string& name);
	double dData(const std::string& name);
	std::string sData(const std::string& name);
	bool bData(const std::string& name);
	ConfigNode* mData(const std::string& name);

	bool has(const std::string& name);
	int type(const std::string& name) const;
	std::auto_ptr< std::list<std::string> > keys(int type = CONFIG_NODE_UNDEFINED);

private:
	boost::scoped_ptr<ConfigParser> m_parser;
	boost::scoped_ptr<ConfigNode> m_root;
};

#endif
