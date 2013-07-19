/*
 * composition.h
 *
 *  Created on: Jul 16, 2013
 *      Author: nbingham
 */

#include "instruction.h"

#ifndef composition_h
#define composition_h

struct composition : instruction
{
	composition();
	virtual ~composition();

	list<instruction*> instrs;		// an ordered list of instructions in sequential

	void init(string chp, vspace *vars, string tab, int verbosity);
	void clear();

	vector<int> variant();
	vector<int> active_variant();
	vector<int> passive_variant();

	virtual void insert_instr(int uid, int nid, instruction *instr) = 0;
	virtual void push(instruction *i) = 0;
};

#endif
