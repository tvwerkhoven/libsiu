/*
    config.h -- Configuration file parsing
    Copyright (C) 2004  Guus Sliepen <guus@sliepen.eu.org>

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
/*!
		@file config.h
		@brief Configuration file parsing class
		@author Guus Sliepen (guus@sliepen.eu.org)
		
		This class can be used to read and write configuration files formatted 
		according to 
		
*/

#ifndef HAVE_CONFIGCLASS_H
#define HAVE_CONFIGCLASS_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#include <inttypes.h>
#include <stdexcept>
#include <string>
#include <map>

#include "path++.h"
#include "format.h"

class config {
	std::map<std::string, std::string> variables;
	std::string pvar(const std::string &var) {
		if (prefix != "") return prefix + "." + var;
		else return var;
	}
	
public:
	std::string filename;
	std::string prefix;
	bool autosave;

	config(const Path &path, const std::string &prefix = ""): filename(path.str()), prefix(prefix), autosave(false) { parse(); }
	config(const std::string &filename, const std::string &prefix = ""): filename(filename), prefix(prefix), autosave(false) { parse(); }
	config(): prefix(""), autosave(false) {};
	~config() { if(autosave) write(); }

	/*! @brief Parse configuration from disk.
	 
	 @param [in] &path Path to read file from.
	 @param [in] &prefix Only parse variables starting with this prefix.
	 */
	void parse(const Path &path, const std::string &prefix = "") { this->filename = path.str(); this->prefix = prefix; parse(); }
	void parse(const std::string &filename, const std::string &prefix = "") { this->filename = filename; this->prefix = prefix; parse(); }
	void parse();
	
	/*! @brief Write configuration to disk. 
	 
	 N.B. This method overwrites the file, and does not retain variable order.
	 
	 @param [in] &path Path to write file to (if not already specified)
	 @param [in] &prefix Prefix to prepend (if not already specified)
	 */
	void write(const Path &path, const std::string &prefix = "") { this->filename = path.str(); this->prefix = prefix; write(); }
	void write(const std::string &filename, const std::string &prefix = "") { this->filename = filename; this->prefix = prefix; write(); }
	void write();
	
	/*! @brief Update this configuration with values from &cfg
	 
	 Update values that are different in cfg. If cfg has a prefix, only update 
	 variables with this prefix.
	 
	 This can be useful when using multiple configuration objects linked to one 
	 file and one wants to write the changed variables back to file. Simply
	 writing each configuration object will not work as because they each 
	 overwrite the previous file. This method can be used to update a master 
	 configuration object with the changes in the other objects which can then 
	 be saved to disk.
	 
	 @param [in] *cfg Other configuration to update with
	 */
	void update(config *cfg);
	void update(config &cfg) { update(&cfg); }

	class exception: public std::runtime_error {
		public:
		exception(const std::string reason): runtime_error(reason) {}
	};

	void require(const std::string &var) { if(variables.find(pvar(var)) == variables.end()) throw exception("Variable " + pvar(var) + " is required"); }
	bool getbool(const std::string &var) { require(pvar(var)); return variables[pvar(var)] == "yes" || variables[pvar(var)] == "true"; }
	int getchoice(const std::string &var, const std::map<std::string, int> &choices);
	int getint(const std::string &var) { require(var); return strtoll(variables[pvar(var)].c_str(), NULL, 0); }
	double getdouble(const std::string &var) { require(var); return atof(variables[pvar(var)].c_str()); }
	uint16_t getuint16(const std::string &var) { require(var); return (uint16_t) strtoumax(variables[pvar(var)].c_str(), NULL, 0); }
	int16_t getint16(const std::string &var) { require(var); return (int16_t) strtoimax(variables[pvar(var)].c_str(), NULL, 0); }
	uint32_t getuint32(const std::string &var) { require(var); return (uint32_t) strtoumax(variables[pvar(var)].c_str(), NULL, 0); }
	int32_t getint32(const std::string &var) { require(var); return (int32_t) strtoimax(variables[pvar(var)].c_str(), NULL, 0); }
	uint64_t getuint64(const std::string &var) { require(var); return (uint64_t) strtoumax(variables[pvar(var)].c_str(), NULL, 0); }
	int64_t getint64(const std::string &var) { require(var); return (int64_t) strtoimax(variables[pvar(var)].c_str(), NULL, 0); }
	std::string getstring(const std::string &var) { require(var); return variables[pvar(var)]; }

	bool exists(const std::string &var) { return variables.find(pvar(var)) != variables.end(); }
	std::map<std::string, std::string> getall() const { return variables; }
	
	bool getbool(const std::string &var, bool def) { if(!exists(var)) return def; else return variables[pvar(var)] == "yes" || variables[pvar(var)] == "true"; }
	int getchoice(const std::string &var, const std::map<std::string, int> &choices, int def);
	int getint(const std::string &var, int def) { if(!exists(var)) return def; else return strtoll(variables[pvar(var)].c_str(),0, 0); }
	double getdouble(const std::string &var, double def) { if(!exists(var)) return def; else return atof(variables[pvar(var)].c_str()); }
	uint16_t getuint16(const std::string &var, uint16_t def) { if(!exists(var)) return def; else return (uint16_t) strtoumax(variables[pvar(var)].c_str(), 0, 0); }
	int16_t getint16(const std::string &var, int16_t def) { if(!exists(var)) return def; else return (int16_t) strtoimax(variables[pvar(var)].c_str(), 0, 0); }
	uint32_t getuint32(const std::string &var, uint32_t def) { if(!exists(var)) return def; else return (uint32_t) strtoumax(variables[pvar(var)].c_str(), 0, 0); }
	int32_t getint32(const std::string &var, int32_t def) { if(!exists(var)) return def; else return (int32_t) strtoimax(variables[pvar(var)].c_str(), 0, 0); }
	uint64_t getuint64(const std::string &var, uint64_t def) { if(!exists(var)) return def; else return (uint64_t) strtoumax(variables[pvar(var)].c_str(), 0, 0); }
	int64_t getint64(const std::string &var, int64_t def) { if(!exists(var)) return def; else return (int64_t) strtoimax(variables[pvar(var)].c_str(), 0, 0); }
	std::string getstring(const std::string &var, const std::string def) { if(!exists(var)) return def; else return variables[pvar(var)]; }

	void set(const std::string &var, bool value) { variables[pvar(var)] = value ? "yes" : "no"; }
	void set(const std::string &var, double value) { variables[pvar(var)] = format("%lg", value); }
	void set(const std::string &var, uint16_t value) { variables[pvar(var)] = format("%"PRIu16, value); }
	void set(const std::string &var, int16_t value) { variables[pvar(var)] = format("%"PRIi16, value); }
	void set(const std::string &var, uint32_t value) { variables[pvar(var)] = format("%"PRIu32, value); }
	void set(const std::string &var, int32_t value) { variables[pvar(var)] = format("%"PRIi32, value); }
	void set(const std::string &var, uint64_t value) { variables[pvar(var)] = format("%"PRIu64, value); }
	void set(const std::string &var, int64_t value) { variables[pvar(var)] = format("%"PRIi64, value); }
	void set(const std::string var, const std::string &value) { variables[pvar(var)] = value; }
	void setchoice(const std::string &var, const std::map<std::string, int> &choices, int value);
};

#endif // HAVE_CONFIGCLASS_H
