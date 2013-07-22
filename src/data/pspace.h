/*
 * pspace.h
 *
 *  Created on: Mar 14, 2013
 *      Author: nbingham
 */

#include "path.h"

#ifndef pspace_h
#define pspace_h

struct path_space
{
	path_space();
	path_space(int s);
	path_space(path p);
	~path_space();

	list<path> paths;
	path total;

	int size();
	void merge(path_space s);
	void push_back(path p);
	list<path>::iterator begin();
	list<path>::iterator end();
	void clear();
	void zero(int i);
	void zero(vector<int> i);

	int coverage_count(int n);
	int coverage_count(vector<int> n);
	vector<int> coverage_maxes();
	int coverage_max();
	path get_mask();
	void apply_mask(path m);
	path_space inverse();
	path_space coverage(int n);
	path_space avoidance(int n);

	path_space &operator=(path_space s);

	path &operator[](int i);
};

ostream &operator<<(ostream &os, path_space p);

#endif