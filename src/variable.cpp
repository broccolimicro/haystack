/*
 * variable.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "variable.h"
#include "common.h"

variable::variable()
{
	name = "";
	type = "";
	width = 0;
	last = "iX";
	reset = "iX";
	fixed = false;
}

variable::variable(string n, string t, value r, uint16_t w)
{
	name = n;
	type = t;
	width = w;
	last = "iX";
	reset = r;
	fixed = true;
}

variable::variable(string raw, string tab, int verbosity)
{
	parse(raw, tab, verbosity);
}

variable::~variable()
{
	name = "";
	type = "";
	width = 0;
	last = "iX";
	reset = "iX";
	fixed = false;
}

variable &variable::operator=(variable v)
{
	name = v.name;
	type = v.type;
	width = v.width;
	last = v.last;
	reset = v.reset;
	fixed = v.fixed;
	return *this;
}

// TODO we need to handle the case where we instantiate a process
void variable::parse(string raw, string tab, int verbosity)
{
	last = "iX";
	reset = "iX";

	chp = raw;

	size_t width_start = chp.find_first_of("< ");
	size_t name_start = chp.find_first_of("> ");
	size_t reset_start = chp.find(":=");

	if (verbosity >= VERB_PARSE)
		cout << tab << "Variable: " << chp << endl;

	if (reset_start != chp.npos)
	{
		name = chp.substr(name_start+1, reset_start - (name_start+1));
		reset = value(chp.substr(reset_start+2), false);
		if (reset.data[1] == 'x')				// hexadecimal e.g. 0xFEEDFACE
			reset.data = hex_to_bin(reset.data.substr(2));
		else if (reset.data[1] == 'b')			// binary      e.g. 0b01100110
			reset.data = reset.data.substr(2);
		else									// decimal     e.g. 20114
			reset.data = dec_to_bin(reset.data);
	}
	else
		name = chp.substr(name_start+1);

	type = chp.substr(0, width_start);
	if (chp.find_first_of("<>") != chp.npos)
	{
		fixed = true;
		width = atoi(chp.substr(width_start+1, name_start - (width_start+1)).c_str());
	}
	else
	{
		fixed = false;
		width = reset.data.length();
	}

	if (verbosity >= VERB_PARSE)
	{
		cout << tab << "\tName:  " << name << endl;
		cout << tab << "\tType:  " << type << endl;
		cout << tab << "\tWidth: " << width << endl;
		cout << tab << "\tReset:  " << reset << endl;
	}
}

ostream &operator<<(ostream &os, variable s)
{
    os << s.name << "<" << s.width << ">";

    return os;
}

