/*
 * instruction.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "instruction.h"
#include "common.h"

instruction::instruction()
{
	chp = "";
	uid = "";
	tab = "";
	_kind = "instruction";
	verbosity = VERB_SUPPRESS;
}

instruction::~instruction()
{
	_kind = "instruction";
}

instruction &instruction::operator=(instruction i)
{
	chp = i.chp;
	uid = i.uid;
	states = i.states;
	rules = i.rules;
	tab = i.tab;
	verbosity = i.verbosity;
	return *this;
}

string instruction::kind()
{
	return _kind;
}

void instruction::print_state_space()
{
	map<string, space>::iterator i;
	if (verbosity >= VERB_PARSE)
	{
		cout << tab << "State Space" << endl;

		for (i = states.begin(); i != states.end(); i++)
			cout << tab + "\t" << i->second << endl;
		cout << endl;
	}
}

void instruction::print_prs()
{
	list<rule>::iterator i;
	if (verbosity >= VERB_PRSALG)
	{
		cout << tab << "Production Rules" << endl;

		for (i = rules.begin(); i != rules.end(); i++)
			cout << tab + "\t" << *i << endl;
	}
}
