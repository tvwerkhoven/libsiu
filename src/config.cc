/*
    config.cc -- Configuration file parsing
    Copyright (C) 2004-2006  Guus Sliepen <guus@sliepen.eu.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <fstream>
#include <string>

#include "config.h"

using namespace std;

void config::parse() {
	ifstream file(filename.c_str());
	string line, variable, value;
	size_t b, e;

	while(getline(file, line)) {
		b = line.find_first_not_of(" \t", 0);
		if(b == string::npos)
			continue;
		if(line[b] == '#')
			continue;
		e = line.find_first_of(" \t", b);
		if(e == string::npos)
			e = line.size();
		variable = line.substr(b, e);
		b = line.find_first_not_of(" =\t", e);
		if(b == string::npos)
			continue;
		e = line.find_last_not_of(" \t");
		if(e == string::npos)
			e = line.size();
		value = line.substr(b, e);
		variables[variable] = value;
	}	
}

void config::write() {
	ofstream file(filename.c_str());

	for(map<string, string>::const_iterator i = variables.begin(); i != variables.end(); ++i)
		file << i->first << " = " << i->second << "\n";
}

void config::update(config *cfg) {
	map<string, string> other_vars = cfg->getall();
	string pref = cfg->prefix;
	
	// Loop over other variables (with prefix), update here
	for(map<string, string>::const_iterator i = other_vars.begin(); i != other_vars.end(); ++i)
		if (i->first.compare(0, pref.length(), pref) == 0)
			variables[i->first] = i->second;
	
}

int config::getchoice(const string &var, const map<string, int> &choices) {
	require(var);
	map<string, int>::const_iterator i = choices.find(variables[pvar(var)]);
	if(i == choices.end())
		throw exception("Unknown choice for variable " + pvar(var));
	return i->second;
}

int config::getchoice(const string &var, const map<string, int> &choices, int def) {
	if(variables[pvar(var)].empty())
		return def;

	map<string, int>::const_iterator i = choices.find(variables[var]);
	if(i == choices.end())
		throw exception("Unknown choice for variable " + pvar(var));
	return i->second;
}

void config::setchoice(const string &var, const map<string, int> &choices, int value) {
	for(map<string, int>::const_iterator i = choices.begin(); i != choices.end(); ++i) {
		if(i->second == value) {
			variables[pvar(var)] = i->first;
			return;
		}
	}
	throw exception("Unknown choice for variable " + pvar(var));
}
