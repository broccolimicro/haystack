/*
 * process.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: Ned Bingham and Nicholas Kramer
 *
 *  DO NOT DISTRIBUTE
 */

#include "../common.h"
#include "../utility.h"
#include "../syntax.h"
#include "../data.h"

#include "process.h"
#include "record.h"
#include "channel.h"

process::process()
{
	name = "";
	_kind = "process";
	verbosity = 0;
	chp = "";
	is_inline = false;
	net.vars = &vars;
}

process::process(string raw, map<string, keyword*> *types, int verbosity)
{
	_kind = "process";
	vars.types = types;
	this->verbosity = verbosity;
	this->chp = raw;
	is_inline = false;
	net.vars = &vars;

	parse(raw);
}

process::~process()
{
	name = "";
	_kind = "process";
	verbosity = 0;
	chp = "";

	vars.clear();
}

process &process::operator=(process p)
{
	def = p.def;
	prs = p.prs;
	vars = p.vars;
	return *this;
}

void process::parse(string raw)
{
	if (raw.compare(0, 7, "inline ") == 0)
	{
		is_inline = true;
		raw = raw.substr(7);
		chp = raw;
	}
	else
		is_inline = false;

	size_t name_start = chp.find_first_of(" ")+1;
	size_t name_end = chp.find_first_of("(");
	size_t input_start = chp.find_first_of("(")+1;
	size_t input_end = chp.find_first_of(")");
	size_t sequential_start = chp.find_first_of("{")+1;
	size_t sequential_end = chp.length()-1;
	string io_sequential;
	string::iterator i, j;

	map<string, variable> temp;
	map<string, variable>::iterator vi, vj;
	map<string, keyword*>::iterator ti;

	name = chp.substr(name_start, name_end - name_start);
	io_sequential = chp.substr(input_start, input_end - input_start);

	if ((verbosity & VERB_BASE_HSE) && (verbosity & VERB_DEBUG))
	{
		cout << "Process:\t" << chp << endl;
		cout << "\tName:\t" << name << endl;
		cout << "\tArgs:\t" << io_sequential << endl;
	}

	for (i = io_sequential.begin(), j = io_sequential.begin(); i != io_sequential.end(); i++)
	{
		if (*(i+1) == ',' || i+1 == io_sequential.end())
		{
			expand_instantiation(NULL, io_sequential.substr(j-io_sequential.begin(), i+1 - j), &vars, &args, "\t", verbosity, false);
			j = i+2;
		}
	}

	string sequential = chp.substr(sequential_start, sequential_end - sequential_start);

	if (!is_inline)
	{
		expand_instantiation(NULL, "__sync call", &vars, &args, "\t", verbosity, false);
		sequential = "[call.r];call.a+;(" + sequential + ");[~call.r];call.a-";
		cout << "LOOK " << sequential << endl;
	}

	def.init(sequential, &vars, "\t", verbosity);

	if ((verbosity & VERB_BASE_HSE) && (verbosity & VERB_DEBUG))
	{
		cout << "\tVariables:" << endl;
		vars.print("\t\t");
		cout << "\tHSE:" << endl;
		def.print_hse("\t\t");
		cout << endl << endl;
	}
}

void process::merge()
{
	def.merge();

	if (verbosity & VERB_MERGED_HSE)
	{
		def.print_hse("");
		cout << endl;
	}
}

// TODO Projection algorithm - when do we need to do projection? when shouldn't we do projection?
void process::project()
{
	if (verbosity & VERB_PROJECTED_HSE)
	{
		def.print_hse("");
		cout << endl;
	}
}

// TODO Process decomposition - How big should we make processes?
void process::decompose()
{
	if (verbosity & VERB_DECOMPOSED_HSE)
	{
		def.print_hse("");
		cout << endl;
	}
}

// TODO Handshaking Reshuffling
void process::reshuffle()
{
	if (verbosity & VERB_RESHUFFLED_HSE)
	{
		def.print_hse("");
		cout << endl;
	}
}

// TODO There is a problem with the interaction of scribe variables with bubbleless reshuffling because scribe variables insert bubbles
void process::generate_states()
{
	cout << "Process" << endl;
	net.vars = &vars;

	vector<int> start;
	start.push_back(net.insert_place(start, map<int, int>(), NULL));
	net.M0.push_back(start[0]);
	net.connect(def.generate_states(&net, start,  map<int, int>()), start);

	do
	{
		net.gen_mutables();
		net.update();
	} while(net.trim());
	net.gen_tails();

	cout << "BRANCHES " << name << endl;
	net.print_branch_ids();
}

void process::insert_state_vars()
{
	map<int, list<vector<int> > >::iterator i;
	list<vector<int> >::iterator lj;
	map<int, int>::iterator m, n;
	int j, k;
	list<path>::iterator li;

	net.gen_conflicts();
	cout << "Conflicts: " << name << endl;
	for (i = net.indistinguishable.begin(); i != net.indistinguishable.end(); i++)
	{
		cout << i->first << ": ";
		for (lj = i->second.begin(); lj != i->second.end(); lj++)
		{
			cout << "{";
			for (j = 0; j < (int)lj->size(); j++)
				cout << (*lj)[j] << " ";
			cout << "} ";
		}
		cout << endl;
	}

	net.gen_conditional_places();

	vector<int> implicants;
	vector<int> arcs;

	for (j = 0; j < (int)net.T.size(); j++)
		if (net.T[j].active)
			implicants.push_back(net.trans_id(j));

	cout << "Implicants: ";
	for (j = 0; j < (int)implicants.size(); j++)
	{
		cout << net.index(implicants[j]) << "{(";
		arcs = net.input_arcs(implicants[j]);
		for (k = 0; k < (int)arcs.size(); k++)
		{
			if (k != 0)
				cout << ", ";
			cout << arcs[k];
		}
		cout << ") -> (";
		arcs = net.output_arcs(implicants[j]);
		for (k = 0; k < (int)arcs.size(); k++)
		{
			if (k != 0)
				cout << ", ";
			cout << arcs[k];
		}
		cout << ")} ";
	}
	cout << endl;

	path up;
	path_space up_paths;
	path down;
	path_space down_paths;
	vector<int> uptrans, downtrans;
	vector<pair<vector<int>, vector<int> > > ip;
	int um, dm;
	int ium, idm;
	for (i = net.indistinguishable.begin(); i != net.indistinguishable.end(); i++)
	{
		for (lj = i->second.begin(); lj != i->second.end(); lj++)
		{
			up_paths = net.get_paths(i->first, *lj, path(net.S.size()));
			down_paths = net.get_paths(*lj, i->first, path(net.S.size()));

			/**
			 * After identifying paths, start at every conditional merge and
			 * check to make sure that there is a path coming from each branch.
			 * If not iterate backwards from the conditional merge along the
			 * paths that do come from a branch until a conflict or a conditional
			 * split is reached and set the paths' values along that part of the
			 * branch to zero. This will prevent cases where you have two
			 * conflicting places on sibling branches and the state variable
			 * insertion algorithm puts transitions after the conflicting places.
			 */
			net.filter_path_space(&up_paths);
			net.filter_path_space(&down_paths);

			up = net.conjugate_path(up_paths.total, implicants);
			down = net.conjugate_path(down_paths.total, implicants);

			cout << "Up:" << endl;
			uptrans.clear();
			while (up_paths.size() > 0 && (ium = net.closest_transition(sample(implicants, up.max()), up.to.front(), path(net.T.size())).second) != -1)
			{
				cout << up << endl;
				uptrans.push_back(ium);
				arcs = net.input_arcs(ium);
				for (j = 0; j < (int)arcs.size(); j++)
					up_paths = up_paths.avoidance(arcs[j]);
				up = net.conjugate_path(up_paths.total, implicants);
			}

			cout << "Down:" << endl;
			downtrans.clear();
			while (down_paths.size() > 0 && (idm = net.closest_transition(sample(implicants, down.max()), down.to.front(), path(net.T.size())).second) != -1)
			{
				cout << down << endl;
				downtrans.push_back(idm);
				arcs = net.input_arcs(idm);
				for (j = 0; j < (int)arcs.size(); j++)
					down_paths = down_paths.avoidance(arcs[j]);
				down = net.conjugate_path(down_paths.total, implicants);
			}
			cout << endl;

			unique(&uptrans, &downtrans);

			if (uptrans.size() == 0 || downtrans.size() == 0)
			{
				cout << "Error: No solution for the conflict set: " << i->first << " -> {";
				for (j = 0; j < (int)lj->size(); j++)
					cout << (*lj)[j] << " ";
				cout << "}." << endl;
			}
			else if (downtrans < uptrans)
				ip.push_back(pair<vector<int>, vector<int> >(downtrans, uptrans));
			else
				ip.push_back(pair<vector<int>, vector<int> >(uptrans, downtrans));
		}
	}

	unique(&ip);

	string vname;
	vector<int> dupfrom;
	int vid;
	for (j = 0; j < (int)ip.size(); j++)
	{
		vname = vars.unique_name("_sv");
		vid   = vars.insert(variable(vname, "node", 1, false));

		um = net.values.mk(vid, 0, 1);
		dm = net.values.mk(vid, 1, 0);

		for (k = 0; k < (int)ip[j].first.size(); k++)
			net.insert_sv_before(ip[j].first[k], um);
		for (k = 0; k < (int)ip[j].second.size(); k++)
			net.insert_sv_before(ip[j].second[k], dm);
	}

	net.trim_branch_ids();
	net.gen_mutables();
	net.update();

	cout << "BRANCHES " << name << endl;
	net.print_branch_ids();


}

void process::generate_prs()
{
	for (int vi = 0; vi < vars.size(); vi++)
		if (vars.get_name(vi).find_first_of("|&~") == string::npos)
			prs.push_back(rule(vi, &net, &vars, verbosity));

	if (verbosity & VERB_BASE_PRS)
	{
		cout << "Production Rules: " << name << endl;
		print_prs();
	}
}

/* TODO: Factoring - production rules should be relatively short.
 * Look for common expressions between production rules and factor them
 * out into their own variable.
 */
void process::factor_prs()
{

	//Not as trivial as seemed on initial exploration.
	//Must find cost function for inserting state variable.
	//Must find benefit for inserting factor
	//Balance size of factored chunk vs how many rules factored from
	//Prioritize longer rules to reduce capacitive driving (i.e. not every 'factor removed' is equal)
	//MUST factor if over cap in series/parallel

	if (verbosity & VERB_FACTORED_PRS)
		print_prs();
}

void process::parse_prs(string raw)
{
	int i, j, k;
	string r, e, v;
	map<string, pair<string, string> > pairs;
	map<string, pair<string, string> >::iterator pi;

	raw = remove_comments(raw);

	for (i = raw.find_first_of("\n\r"), j = 0; i != (int)raw.npos; j = i+1, i = raw.find_first_of("\n\r", j))
	{
		r = raw.substr(j, i-j);
		k = r.find(" -> ");
		if (k != (int)r.npos)
		{
			e = r.substr(0, k);
			v = r.substr(k+4);

			pi = pairs.find(v.substr(0, v.length()-1));
			if (pi != pairs.end())
			{
				if (v[v.length()-1] == '+')
					pi->second.first = e;
				else
					pi->second.second = e;
			}
			else
			{
				if (v[v.length()-1] == '+')
					pairs.insert(pair<string, pair<string, string> >(v.substr(0, v.length()-1), pair<string, string>(e, "")));
				else
					pairs.insert(pair<string, pair<string, string> >(v.substr(0, v.length()-1), pair<string, string>("", e)));
			}
		}
	}

	for (pi = pairs.begin(); pi != pairs.end(); pi++)
	{
		cout << pi->first << ",{" << pi->second.first << ", " << pi->second.second << "}" << endl;
		prs.push_back(rule(pi->second.first, pi->second.second, pi->first, &vars, &net));
	}
}

void process::elaborate_prs()
{
	int ufrom, dfrom;
	int utrans, dtrans;
	int uto, dto;
	vector<int> ot, op;
	vector<int> vl;
	list<list<pair<int, int> > > sat;
	list<list<pair<int, int> > >::iterator si;
	int i, j;

	for (i = 0; i < (int)prs.size(); i++)
	{
		sat = net.values.allsat(net.values.apply_and(prs[i].up, net.values.mk(prs[i].uid, 1, 0)));
		//sat = net.values.allsat(prs[i].up);
		for (si = sat.begin(); si != sat.end(); si++)
		{
			ufrom  = net.new_place(net.values.build(*si), map<int, int>(), NULL);
			utrans = net.insert_transition(ufrom, net.values.mk(prs[i].uid, 0, 1), map<int, int>(), NULL);
			uto    = net.insert_place(utrans, map<int, int>(), NULL);
			net.S[uto].index = net.values.transition(net.S[ufrom].index, net.T[utrans].index);
			net.S[ufrom].active = true;
			net.T[utrans].active = true;
		}

		sat = net.values.allsat(net.values.apply_and(prs[i].down, net.values.mk(prs[i].uid, 0, 1)));
		//sat = net.values.allsat(prs[i].down);
		for (si = sat.begin(); si != sat.end(); si++)
		{
			dfrom  = net.new_place(net.values.build(*si), map<int, int>(), NULL);
			dtrans = net.insert_transition(dfrom, net.values.mk(prs[i].uid, 1, 0), map<int, int>(), NULL);
			dto    = net.insert_place(dtrans, map<int, int>(), NULL);
			net.S[dto].index = net.values.transition(net.S[dfrom].index, net.T[dtrans].index);
			net.S[dfrom].active = true;
			net.T[dtrans].active = true;
		}
	}

	net.merge_conflicts();

	// Try to guess the environment's behavior
	/*vector<int> inactive;
	vector<int> active;
	map<string, variable>::iterator vi;
	for (vi = vars.global.begin(); vi != vars.global.end(); vi++)
	{
		if (!vi->second.driven)
			inactive.push_back(vi->second.uid);
		else
			active.push_back(vi->second.uid);
	}

	for (i = 0; i < (int)net.S.size(); i++)
		for (j = 0; j < (int)net.S.size(); j++)
			if (i != j && !net.connected(i, j) && net.values.apply_and(net.values.smooth(net.S[i].index, inactive), net.S[j].index) > 1)
			{
				ufrom = net.values.smooth(net.S[j].index, active);
				utrans = net.insert_transition(i, ufrom, map<int, int>(), NULL);
				net.connect(utrans, j);
			}*/

	net.zip();

	for (int i = 0; i < (int)net.S.size(); i++)
		if (net.output_arcs(i).size() == 0)
		{
			net.remove_place(i);
			i--;
		}
}

void process::print_hse(ostream *fout)
{
	def.print_hse("", fout);
}

void process::print_dot(ostream *fout)
{
	net.print_dot(fout, name);
}

void process::print_petrify()
{
	net.print_petrify(name);
}

void process::print_prs(ostream *fout)
{
	(*fout) << "/* Process " << name << " */" << endl;
	map<string, variable>::iterator vi;
	map<string, keyword*>::iterator ki;
	for (size_t i = 0; i < prs.size(); i++)
		(*fout) << prs[i];

	(*fout) << "/* Environment */" << endl;
	for (vi = vars.label.begin(); vi != vars.label.end(); vi++)
	{
		ki = vars.types->find(vi->second.type);
		if (ki != vars.types->end() && ki->second != NULL && ki->second->kind() == "channel")
		{
			((channel*)ki->second)->send->print_prs(fout, vi->first + ".", vars.get_driven());
			((channel*)ki->second)->recv->print_prs(fout, vi->first + ".", vars.get_driven());
		}
	}
}
