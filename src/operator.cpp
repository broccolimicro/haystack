/*
 * process.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "process.h"
#include "record.h"
#include "common.h"
#include "channel.h"
#include "operator.h"

operate::operate()
{
	name = "";
	_kind = "operate";
}

operate::operate(string raw, map<string, keyword*> types, map<string, variable> vars, int verbosity)
{
	parse(raw, types, vars, verbosity);
	_kind = "operate";
}

operate::~operate()
{
	name = "";
	_kind = "operate";

	global.clear();
}

operate &operate::operator=(operate p)
{
	def = p.def;
	prs = p.prs;
	global = p.global;
	return *this;
}

void operate::parse(string raw, map<string, keyword*> types, map<string, variable> vars, int verbosity)
{
	chp = raw;

	size_t name_start = 0;
	size_t name_end = chp.find_first_of("(");
	size_t input_start = chp.find_first_of("(")+1;
	size_t input_end = chp.find_first_of(")");
	size_t block_start = chp.find_first_of("{")+1;
	size_t block_end = chp.length()-1;
	string io_block;
	string::iterator i, j;

	map<string, variable> temp;
	map<string, variable>::iterator vi, vj;
	map<string, keyword*>::iterator ti;

	cout << "Operator:\t" << chp << endl;

	global.insert(vars.begin(), vars.end());

	name = chp.substr(name_start, name_end - name_start);
	io_block = chp.substr(input_start, input_end - input_start);

	if (verbosity >= VERB_PARSE)
	{
		cout << "\tName:\t" << name << endl;
		cout << "\tInputs:\t" << io_block << endl;
	}

	for (i = io_block.begin(), j = io_block.begin(); i != io_block.end(); i++)
	{
		if (*(i+1) == ',' || i+1 == io_block.end())
		{
			expand_instantiation(io_block.substr(j-io_block.begin(), i+1 - j), types, &global, &label, &input, "\t", verbosity, false);
			j = i+2;
		}
	}

	def.init(chp.substr(block_start, block_end - block_start), types, &global, &label, "\t", verbosity);
	def.print_hse();
}
