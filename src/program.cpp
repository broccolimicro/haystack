#include "program.h"
#include "utility.h"

program::program()
{
	type_space.insert(pair<string, keyword*>("int", new keyword("int")));
}

program::program(string chp, int verbosity)
{
	parse(chp, verbosity);
}

program::~program()
{
	map<string, keyword*>::iterator i;
	for (i = type_space.begin(); i != type_space.end(); i++)
		delete i->second;

	type_space.clear();
}

program &program::operator=(program p)
{
	type_space = p.type_space;
	prs_up = p.prs_up;
	prs_down = p.prs_down;
	errors = p.errors;
	return *this;
}


void program::parse(string chp, int verbosity)
{
	//TODO: Lost information in statespace from guard (example a xor b) not incorperated. Copy into PRS?
	//TODO: THIS BREAKS IF THERE ARE NO IMPLICANTS FOR A OUTPUT
	//TODO: Logic minimization
	//TODO: Figure out indistinguishable states
	//TODO: Add state variables
	//TODO: Explore State Variable Factorization
	string::iterator i, j;
	string cleaned_chp = "";
	string word;
	string error;
	int error_start, error_len;

	process *p;
	operate *o;
	record *r;
	channel *c;

	// Define the basic types. In this case, 'int'
	type_space.insert(pair<string, keyword*>("int", new keyword("int")));

	//Remove line comments:
	size_t comment_begin = chp.find("//");
	size_t comment_end = chp.find("\n", comment_begin);
	while (comment_begin != chp.npos && comment_end != chp.npos){
		chp = chp.substr(0,comment_begin) + chp.substr(comment_end);
		comment_begin = chp.find("//");
		comment_end = chp.find("\n", comment_begin);
	}

	//Remove block comments:
	comment_begin = chp.find("/*");
	comment_end = chp.find("*/");
	while (comment_begin != chp.npos && comment_end != chp.npos){
		chp = chp.substr(0,comment_begin) + chp.substr(comment_end+2);
		comment_begin = chp.find("/*");
		comment_end = chp.find("*/");
	}

	// remove extraneous whitespace
	for (i = chp.begin(); i != chp.end(); i++)
	{
		if (!sc(*i))
			cleaned_chp += *i;
		else if (nc(*(i-1)) && (i == chp.end()-1 || nc(*(i+1))))
			cleaned_chp += ' ';
	}

	// split the program into records and processes
	int depth[3] = {0};
	for (i = cleaned_chp.begin(), j = cleaned_chp.begin(); i != cleaned_chp.end(); i++)
	{
		if (*i == '(')
			depth[0]++;
		else if (*i == '[')
			depth[1]++;
		else if (*i == '{')
			depth[2]++;
		else if (*i == ')')
			depth[0]--;
		else if (*i == ']')
			depth[1]--;
		else if (*i == '}')
			depth[2]--;

		// Are we at the end of a record or process definition?
		if (depth[0] == 0 && depth[1] == 0 && depth[2] == 0 && *i == '}')
		{
			// Make sure this isn't vacuous
			if (i-j+1 > 0)
			{
				// Is this a process?
				if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "process ") == 0)
				{
					p = new process(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
					type_space.insert(pair<string, process*>(p->name, p));
				}
				// Is this an operator?
				else if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "operator") == 0)
				{
					o = new operate(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
					type_space.insert(pair<string, operate*>(o->name, o));
				}
				// This isn't a process, is it a record?
				else if (cleaned_chp.compare(j-cleaned_chp.begin(), 7, "record ") == 0)
				{
					r = new record(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
					type_space.insert(pair<string, record*>(r->name, r));
				}
				// Is it a channel definition?
				else if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "channel ") == 0)
				{
					c = new channel(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
					type_space.insert(pair<string, channel*>(c->name, c));
					type_space.insert(pair<string, operate*>(c->name + "." + c->send.name, &c->send));
					type_space.insert(pair<string, operate*>(c->name + "." + c->recv.name, &c->recv));
					type_space.insert(pair<string, operate*>(c->name + "." + c->probe.name, &c->probe));
				}
				// This isn't either a process or a record, this is an error.
				else
				{
					error = "Error: CHP block outside of process.\nIgnoring block:\t";
					error_start = j-cleaned_chp.begin();
					error_len = min(min(cleaned_chp.find("process ", error_start), cleaned_chp.find("record ", error_start)), cleaned_chp.find("channel ", error_start)) - error_start;
					error += cleaned_chp.substr(error_start, error_len);
					cout << error << endl;
					j += error_len;

					// Make sure we don't miss the next record or process though.
					if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "process ") == 0)
					{
						p = new process(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
						type_space.insert(pair<string, process*>(p->name, p));
					}
					else if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "operator") == 0)
					{
						o = new operate(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
						type_space.insert(pair<string, operate*>(o->name, o));
					}
					else if (cleaned_chp.compare(j-cleaned_chp.begin(), 7, "record ") == 0)
					{
						r = new record(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
						type_space.insert(pair<string, record*>(r->name, r));
					}
					else if (cleaned_chp.compare(j-cleaned_chp.begin(), 8, "channel ") == 0)
					{
						c = new channel(cleaned_chp.substr(j-cleaned_chp.begin(), i-j+1), type_space, verbosity);
						type_space.insert(pair<string, channel*>(c->name, c));
						type_space.insert(pair<string, operate*>(c->name + "." + c->send.name, &c->send));
						type_space.insert(pair<string, operate*>(c->name + "." + c->recv.name, &c->recv));
						type_space.insert(pair<string, operate*>(c->name + "." + c->probe.name, &c->probe));
					}
				}
			}
			j = i+1;
		}
	}

	prgm = (parallel*)expand_instantiation("main _()", type_space, &vars, NULL, "", verbosity, true);

	//At this point in the program, 'parsing' is done. Launching State Space Gen

	vars.insert(variable("Reset", "int", value("0"), 1, false));
	vars.insert(variable("Reset_", "int", value("1"), 1, false));

	cout << "Generating State Space" << endl;
	state sr, s;
	for (map<string, variable>::iterator ri = vars.global.begin(); ri != vars.global.end(); ri++)
	{
		if (ri->first == "Reset")
			sr.values.push_back(value("1"));
		else if (ri->first == "Reset_")
			sr.values.push_back(value("0"));
		else
			sr.values.push_back(value("X"));

		s.values.push_back(ri->second.reset);
	}
	space.states.push_back(sr);
	space.states.push_back(s);
	trans.insert_edge(0, 1, "Reset");
	prgm->generate_states(&space, &trans, 1);

	//Generate states is done. Launching post-state info gathering

	//The whole program has states now!

	if(STATESP_CO)
	{
		print_space_to_console();
	}
	if(STATESP_GR)
	{
		print_space_graph_to_console();
	}
	//Generate+print diff_space
	state_space diff_space = delta_space_gen(space, trans);
	print_diff_space_to_console(diff_space);

	cout << vars << endl;

	prgm->print_hse();
	cout << endl;
	//Create an up and down PRS for each variable  (UID indexed)
	prs_up.resize(vars.global.size());
	prs_down.resize(vars.global.size());
	//Inserting names into each PRS
	for(int i = 0; i < (int)prs_up.size(); i++)
	{
		prs_up[i].right = vars.get_name(i)+"+";
		prs_down[i].right = vars.get_name(i)+"-";
	}


	//Find the implicants of the diff space
	for(int i = 0; i < diff_space.size();i++)
	{
		for(int j = 0; j< diff_space[i].size(); j++)
		{
			if((diff_space[i])[j].data == "1")
			{
				if(diff_space[i].tag!=-1)	//Output variable needs to fire high
					prs_up[j].implicants.push_back(space[diff_space[i].tag]);
			}
			if((diff_space[i])[j].data == "0")
			{
				if(diff_space[i].tag!=-1)	//Output variable needs to fire low
					prs_down[j].implicants.push_back(space[diff_space[i].tag]);
			}

		}
	}
	merge_implicants();
	print_prs();
	cout << "Done!" << endl<< endl << endl;
}


void print_line(int from, graph *trans)
{
	int i;
	if(from >= (int)trans->edges.size())
		trans->edges.resize(from+1, vector<int>());

	cout << from << ": ";
	for (i = 0; i < (int)trans->edges[from].size(); i++)
		cout << (trans->edges[from])[i] << " ";
	cout << endl;
}

void print_line_dot(int from, state_space *spaces, graph *trans) // Print a line following .dot graphvis formatting
{
	int i;
	if(from >= (int)trans->edges.size())
		trans->edges.resize(from+1, vector<int>());

	//Each edge should be: 	"Node 1" -> "Node 2" [ label = "trans" ];
	for (i = 0; i < (int)trans->edges[from].size(); i++)
	{
		//	"Node 1" -> "Node 2" [ label = "trans" ];
		//"Node 1" ->
		cout << "\t\"" << from << ":" << (*spaces)[from] << "\"" << " -> ";
		//"Node 2
		cout << "\"" << trans->edges[from][i] << ":" << (*spaces)[trans->edges[from][i]];
		//" [ label = "trans" ];
		cout << "\" [ label = \"" << (trans->transitions[from])[i] << "\" ];" << endl;
	}

}
void print_line_with_trans(int from, graph *trans)
{
	int i, j;
	if(from >= (int)trans->edges.size())
		trans->edges.resize(from+1, vector<int>());

	cout << from << ":";
	for (i = 0; i < 4 - (int)log10((double)max(from, 1)); i++)
		cout << " ";
	j = 0;
	for (i = 0; i < (int)trans->edges[from].size(); i++)
	{
		cout << trans->edges[from][i] << " ";
		j += (int)log10((double)max(trans->edges[from][i], 1)) + 2;
	}
	for (i = 0; i < 10 - j; i++)
		cout << " ";
	for (i = 0; from < (int)trans->transitions.size() && i < (int)trans->transitions[from].size(); i++)
		cout << (trans->transitions[from])[i] << " ";

	cout << endl;
}
void program::print_space_to_console()
{
	//Print space (for debugging purposes)
	cout << endl << endl << "\tState space:" << endl;
	for(int i = 0; i < space.size(); i++)
	{
		cout << "\t "<< space[i] << "  ";
		print_line_with_trans(i, &trans);
	}
	cout << endl << endl;
	//cout << "Current connections: " << endl;
	//cout << (*trans);
}
void print_diff_space_to_console(state_space diff_space)
{
	//Print space (for debugging purposes)
	cout << endl << endl << "\tDiff state space:" << endl;
	for(int i = 0; i < diff_space.size(); i++)
	{
		cout << "\t "<< diff_space[i] << "  ";
		cout << diff_space[i].tag << endl;
	}
	cout << endl << endl;
	//cout << "Current connections: " << endl;
	//cout << (*trans);
}

void program::print_space_graph_to_console()
{
	//Print space (for debugging purposes)
	cout << endl << endl << "\t.dot formatted graph:" << endl;
	cout << "digraph finite_state_machine {"<< endl << "\tgraph [ label = \"\\n\\nState space graph!\" ];" << endl;
	if (!GRAPH_VERT)
		cout <<"\trankdir=LR;"<<endl;
	cout <<"\tnode [shape = ellipse];"<<endl;
	cout <<"\tgraph [ dpi =" << GRAPH_DPI << " ];" << endl;
	for(int i = 0; i < space.size(); i++)
	{
		//cout << "\t "<< space[i] << "  ";
		print_line_dot(i, &space, &trans);
	}
	cout << "}" << endl << endl;

}

void program::merge_implicants()
{
	//Print out implicants
	map<string, variable>::iterator globali = vars.global.begin();
	for (int i = 0; i< (int)prs_up.size(); i++, globali++)
	{
		for(list<state>::iterator upi = prs_up[i].implicants.begin(); upi!=prs_up[i].implicants.end(); upi++)
		{
			if(!is_all_x(*upi))
			{
				prs_up[i].left += "(";
				bool first = true;
				for(int j = 0; j < upi->size(); j++)
				{
					if ( ((*upi)[j].data != "X") && (vars.get_name(j) != prs_up[i].right.substr(0,prs_up[i].right.size()-1)))
					{
						if(!first)
							prs_up[i].left += " & ";
						else
							first = false;

						//if(vars.get_name(j) != prs_up[i].right.substr(0,prs_up[i].right.size()-1))
						//{
						if((*upi)[j].data == "0")
							prs_up[i].left += "~";
						prs_up[i].left += vars.get_name(j);
						//}
					}
				}
				prs_up[i].left += ")";
				prs_up[i].left += " | ";
			}
		}
		if(prs_up[i].left.size() >= 3)
			prs_up[i].left = prs_up[i].left.substr(0, prs_up[i].left.size() - 3);
		for(list<state>::iterator downi = prs_down[i].implicants.begin(); downi!=prs_down[i].implicants.end(); downi++)
		{
			if(!is_all_x(*downi))
			{
				prs_down[i].left += "(";
				bool first = true;
				for(int j = 0; j < downi->size(); j++)
				{
					if ( ((*downi)[j].data != "X")  && (vars.get_name(j) != prs_down[i].right.substr(0,prs_down[i].right.size()-1)))
					{
						if (!first)
							prs_down[i].left += " & ";
						else
							first = false;

						//if(vars.get_name(j) != prs_down[i].right.substr(0,prs_down[i].right.size()-1)){
						if((*downi)[j].data == "0")
							prs_down[i].left += "~";
						prs_down[i].left += vars.get_name(j);
						//}

					}
				}
				prs_down[i].left += ")";
				prs_down[i].left += " | ";
			}
		}
		if(prs_down[i].left.size() >= 3)
			prs_down[i].left = prs_down[i].left.substr(0, prs_down[i].left.size() - 3);

	}
}

void program::print_prs()
{
	//Print out implicants
	map<string, variable>::iterator globali = vars.global.begin();

	cout << endl << endl << endl << "Production Rules: " << endl;

	for (int i = 0; i< (int)prs_up.size(); i++, globali++)
	{
		if (prs_up[i].left != "")
			cout << prs_up[i].left << " -> " << prs_up[i].right << endl;
		if (prs_down[i].left != "")
			cout << prs_down[i].left << " -> " << prs_down[i].right << endl;

	}

}


state_space delta_space_gen(state_space spaces, graph trans)
{
	state_space delta_space;
	state leaving_state, incoming_state, result_state;


	for(int i = 0; i < spaces.size(); i++)
	{
		if(i >= (int)trans.edges.size())
		{
			trans.edges.resize(i+1, vector<int>());
			cout << "Does this ever occur???" << endl;
		}

		for (int j = 0; j < (int)trans.edges[i].size(); j++)
		{
			//Node 1
			leaving_state = spaces[i];
			//Node 2
			incoming_state = spaces[trans.edges[i][j]];
			result_state = diff(leaving_state,incoming_state);
			if(incoming_state.prs)
				result_state.tag = i;
			else
				result_state.tag = -1;

			if(incoming_state.prs || SHOW_ALL_DIFF_STATES)
				delta_space.states.push_back(result_state);
		}
	}
	//TODO: return a graph, too, so that the states mean something mathematically?
	return delta_space;

}
