/*
 * channel.h
 *
 *  Created on: Oct 23, 2012
 *      Author: Ned Bingham
 */

#include "common.h"
#include "keyword.h"
#include "variable.h"
#include "record.h"
#include "process.h"

#ifndef channel_h
#define channel_h

/* This structure represents a structure or channel. A channel
 * contains a bunch of member variables that help you index
 * segments of bits within the multibit signal.
 */
struct channel : record
{
	channel();
	channel(string chp, map<string, keyword*> types, string tab);
	~channel();

	process send;
	process recv;

	channel &operator=(channel r);
	void parse(string chp, map<string, keyword*> types, string tab);
};

ostream &operator<<(ostream &os, channel s);

#endif
