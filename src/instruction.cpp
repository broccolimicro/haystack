/*
 * instruction.cpp
 *
 *  Created on: Oct 28, 2012
 *      Author: Ned Bingham
 */

#include "instruction.h"
#include "common.h"

instruction::instruction()
{
	_kind = "instruction";
}
instruction::instruction(string raw, map<string, keyword*> types, map<string, variable*> vars, string tab)
{
	_kind = "instruction";
	parse(raw, types, vars, tab);
}
instruction::~instruction()
{
	_kind = "instruction";
}

instruction &instruction::operator=(instruction i)
{
	chp = i.chp;
	result = i.result;
	return *this;
}

string instruction::kind()
{
	return _kind;
}

state expr_eval(string raw){
	// TODO:
	//Add paren!
	//Read variables
	//Less than, greater than
	//Add negative support for variables before operators besides +- (ex. a*-b should not be a*0-b)
	//NEEDS TESTING!
	//Test functionality

	// Supported operators: + - * / & | << >> == != <= >= < >
	//Find occurrences in strings!
	// Weakest bind set below
	// |
	int first_or = raw.find("|");

	// &
	int first_and = raw.find("&");

	//== !=
	int first_equal = raw.find("==");
	int first_nequal = raw.find("!=");
	int first_comp = raw.npos;
	if((first_equal != raw.npos)&&(first_equal < first_nequal)){
		first_comp = first_equal;
	}else if(first_nequal != raw.npos){
		first_comp = first_nequal;
	}

	//<= >=
	int first_ltequal = raw.find("<=");
	int first_gtequal = raw.find(">=");
	int first_ltgtequal = raw.npos;
	if((first_ltequal != raw.npos)&&(first_ltequal < first_gtequal)){
		first_ltgtequal = first_ltequal;
	}else if(first_gtequal != raw.npos){
		first_ltgtequal = first_gtequal;
	}

	//< >
	//Not yet supported

	// << >>
	int first_shift_l = raw.find("<<");
	int first_shift_r = raw.find(">>");
	int first_shift = raw.npos;
	if((first_shift_l != raw.npos)&&(first_shift_l < first_shift_r)){
		first_shift = first_shift_l;
	}else if(first_shift_r != raw.npos){
		first_shift = first_shift_r;
	}

	//+ -
	int first_addsub = raw.find_first_of("+-");

	//* /
	int first_muldiv = raw.find_first_of("*/");

	//()
	//Not yet supported

	//Strongest bind set above

	state result; 	//The value we are to return.

	//Any ors?
	if(first_or != raw.npos){
		cout << "Doing an or between " + raw.substr(0,first_or) + " and " +raw.substr(first_or+1) << endl;
		result = expr_eval(raw.substr(0,first_or)) | expr_eval(raw.substr(first_or+1));
		cout << "Result of or is: " + result.data << endl;
		return result;
	}
	//Any ands?
	if(first_and != raw.npos){
		cout << "Doing an and between " + raw.substr(0,first_and) + " and " +raw.substr(first_and+1) << endl;
		result = expr_eval(raw.substr(0,first_and)) & expr_eval(raw.substr(first_and+1));
		cout << "Result of and is: " + result.data << endl;
		return result;
	}
	//Any comps?
	if(first_comp != raw.npos){
		if(raw[first_comp] == '='){		//It is a '=='!
			cout << "Doing an == between " + raw.substr(0,first_comp) + " and " +raw.substr(first_comp+2) << endl;
			result = expr_eval(raw.substr(0,first_comp)) == expr_eval(raw.substr(first_comp+2));
			cout << "Result of == is: " + result.data << endl;
			return result;
		}else{							//It is a '!='!
			cout << "Doing an != between " + raw.substr(0,first_comp) + " and " +raw.substr(first_comp+2) << endl;
			result = expr_eval(raw.substr(0,first_comp)) != expr_eval(raw.substr(first_comp+2));
			cout << "Result of != is: " + result.data << endl;
			return result;
		}
	}
	//Any ltgtequal?
	if(first_ltgtequal != raw.npos){
		if(raw[first_ltgtequal] == '>'){		//It is a '>='!
			cout << "Doing an >= between " + raw.substr(0,first_ltgtequal) + " and " +raw.substr(first_ltgtequal+2) << endl;
			result = expr_eval(raw.substr(0,first_ltgtequal)) >= expr_eval(raw.substr(first_ltgtequal+2));
			cout << "Result of >= is: " + result.data << endl;
			return result;
		}else{							//It is a '<='!
			cout << "Doing an <= between " + raw.substr(0,first_ltgtequal) + " and " +raw.substr(first_ltgtequal+2) << endl;
			result = expr_eval(raw.substr(0,first_ltgtequal)) <= expr_eval(raw.substr(first_ltgtequal+2));
			cout << "Result of <= is: " + result.data << endl;
			return result;
		}
	}
	//Any >> <<?
	/*
	// NOT SUPPORTED! WAITING FOR NED TO MAKE << >> A STATE STATE OPERATION
	if(first_shift != raw.npos){
		if(raw[first_shift] == '>'){		//It is a '>>'!
			cout << "Doing an >> between " + raw.substr(0,first_shift) + " and " +raw.substr(first_shift+2) << endl;
			result = expr_eval(raw.substr(0,first_shift)) >> expr_eval(raw.substr(first_shift+2));
			cout << "Result of >> is: " + result.data << endl;
			return result;
		}else{							//It is a '<<'!
			cout << "Doing an << between " + raw.substr(0,first_shift) + " and " +raw.substr(first_shift+2) << endl;
			result = expr_eval(raw.substr(0,first_shift)) << expr_eval(raw.substr(first_shift+2));
			cout << "Result of << is: " + result.data << endl;
			return result;
		}
	}*/
	//Any +/-?
	if(first_addsub != raw.npos){			//There is a plus or a minus!
		if (raw[first_addsub] == '+'){		//It is a plus!
			//recurse splitting the first occurrence of the weakest binder.
			cout << "Doing an add between " + raw.substr(0,first_addsub) + " and " +raw.substr(first_addsub+1) << endl;
			result = expr_eval(raw.substr(0,first_addsub)) + expr_eval(raw.substr(first_addsub+1));
			cout << "Result of add is: " + result.data << endl;
			return result;
		}else{								//It is a minus sign!
			//We need to handle whether this is a minus or a negative
			if(( first_addsub == 0 ) || (raw[first_addsub-1] == '+') || (raw[first_addsub-1] == '-') || (raw[first_addsub-1] == '*') || (raw[first_addsub-1] == '/')){
				raw = raw.substr(0,first_addsub) + "0" + raw.substr(first_addsub);
				return expr_eval(raw);
				// a * -b + c*d
			}else{			//This negative does not need to be 'fixed'
				//recurse splitting the first occurrence of the weakest binder.
				cout << "Doing an sub between " + raw.substr(0,first_addsub) + " and " +raw.substr(first_addsub+1) << endl;
				result = expr_eval(raw.substr(0,first_addsub)) - expr_eval(raw.substr(first_addsub+1));
				cout << "Result of sub is: " + result.data << endl;
				return result;
			}
		}
	}
	if(first_muldiv != raw.npos){		//There is a mul or a div!
		if(raw[first_muldiv] == '*'){	//It is a mul!
			//recurse splitting the first occurrence of the weakest binder.
			cout << "Doing an mul between " + raw.substr(0,first_muldiv) + " and " +raw.substr(first_muldiv+1) << endl;
			result = expr_eval(raw.substr(0,first_muldiv)) * expr_eval(raw.substr(first_muldiv+1));
			cout << "Result of mul is: " + result.data << endl;
			return result;
		}else{							//It is a div!
			//recurse splitting the first occurrence of the weakest binder.
			cout << "Doing an div between " + raw.substr(0,first_muldiv) + " and " +raw.substr(first_muldiv+1) << endl;
			result = expr_eval(raw.substr(0,first_muldiv)) / expr_eval(raw.substr(first_muldiv+1));
			cout << "Result of div is: " + result.data << endl;
			return result;
		}
	}
	//Failed implementations for legacy
	/*if((first_plus*(first_plus+1) < (first_minus+1)*first_minus) && ((first_plus != raw.npos)||(first_minus != raw.npos))){
//	if((((first_plus < first_minus) && (first_plus!= raw.npos))||(first_minus==raw.npos||first_plus!=raw.npos))&&((first_plus != raw.npos)||(first_minus != raw.npos))){
		//recurse splitting the first occurrence of the weakest binder.
		cout << "Doing an add between " + raw.substr(0,first_plus) + " and " +raw.substr(first_plus+1) << endl;
		result = expr_eval(raw.substr(0,first_plus)) + expr_eval(raw.substr(first_plus+1));
		cout << "Result is: " + result.data << endl;
		return result;
	}else if(first_minus != raw.npos){
		//recurse splitting the first occurrence of the weakest binder.
		cout << "Doing an sub between " + raw.substr(0,first_minus) + " and " +raw.substr(first_minus+1) << endl;
		result = expr_eval(raw.substr(0,first_minus)) - expr_eval(raw.substr(first_minus+1));
		cout << "Result is: " + result.data << endl;
		return result;
	}

	if(((first_mul < first_div)||(first_div==raw.npos||first_mul!=raw.npos))&&((first_mul != raw.npos)||(first_div != raw.npos))){
		//recurse splitting the first occurrence of the weakest binder.
		cout << "Doing an mul between " + raw.substr(0,first_mul) + " and " +raw.substr(first_mul+1) << endl;
		result = expr_eval(raw.substr(0,first_mul)) * expr_eval(raw.substr(first_mul+1));
		cout << "Result is: " + result.data << endl;
		return result;
	}else if((first_mul != raw.npos)||(first_div != raw.npos)){
		//recurse splitting the first occurrence of the weakest binder.
		cout << "Doing an div between " + raw.substr(0,first_div) + " and " +raw.substr(first_div+1) << endl;
		result = expr_eval(raw.substr(0,first_div)) / expr_eval(raw.substr(first_div+1));
		cout << "Result is: " + result.data << endl;
		return result;
	} */
/*
	if((first_plus != raw.npos)||(first_minus != raw.npos)){	//We have adds and/or subtracts!
		if ((first_plus != raw.npos)&&(first_minus != raw.npos)){	//We have at least one add and a sub.
			//evaluate left to right...
			if(first_plus < first_minus){
				//recurse splitting the first occurrence of the weakest binder.
				cout << "Doing an add between " + raw.substr(0,first_plus) + " and " +raw.substr(first_plus+1) << endl;
				result = expr_eval(raw.substr(0,first_plus)) + expr_eval(raw.substr(first_plus+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}else{
				//recurse splitting the first occurrence of the weakest binder.
				cout << "Doing an sub between " + raw.substr(0,first_minus) + " and " +raw.substr(first_minus+1) << endl;
				result = expr_eval(raw.substr(0,first_minus)) - expr_eval(raw.substr(first_minus+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}
		}else{			//We only have adds or subs.
			if(first_plus != raw.npos){		//We only have adds
				cout << "Doing an add between " + raw.substr(0,first_plus) + " and " +raw.substr(first_plus+1) << endl;
				result = expr_eval(raw.substr(0,first_plus)) + expr_eval(raw.substr(first_plus+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}else{				//We only have subs.
				cout << "Doing an sub between " + raw.substr(0,first_minus) + " and " +raw.substr(first_minus+1) << endl;
				result = expr_eval(raw.substr(0,first_minus)) - expr_eval(raw.substr(first_minus+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}
		}
	}

	if((first_mul != raw.npos)||(first_div != raw.npos)){	//We have muls and/or divs!
		if ((first_mul != raw.npos)&&(first_div != raw.npos)){	//We have at least one mul and one div.
			//evaluate left to right...
			if(first_mul < first_div){
				//recurse splitting the first occurrence of the weakest binder.
				cout << "Doing an mul between " + raw.substr(0,first_mul) + " and " +raw.substr(first_mul+1) << endl;
				result = expr_eval(raw.substr(0,first_mul)) * expr_eval(raw.substr(first_mul+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}else{
				//recurse splitting the first occurrence of the weakest binder.
				cout << "Doing an div between " + raw.substr(0,first_div) + " and " +raw.substr(first_div+1) << endl;
				result = expr_eval(raw.substr(0,first_div)) / expr_eval(raw.substr(first_div+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}
		}else{			//We only have muls or divs.
			if(first_mul != raw.npos){		//We only have muls
				cout << "Doing an mul between " + raw.substr(0,first_mul) + " and " +raw.substr(first_mul+1) << endl;
				result = expr_eval(raw.substr(0,first_mul)) * expr_eval(raw.substr(first_mul+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}else{				//We only have divs.
				cout << "Doing an div between " + raw.substr(0,first_div) + " and " +raw.substr(first_div+1) << endl;
				result = expr_eval(raw.substr(0,first_div)) / expr_eval(raw.substr(first_div+1));
				cout << "Result is: " + result.data << endl;
				return result;
			}
		}
	}*/

	// If this is a multi-bit number, then we need to make sure it is correctly parsed
	if (raw[1] == 'x')			// hexadecimal e.g. 0xFEEDFACE
		raw = hex_to_bin(raw.substr(2));

	else if (raw[1] == 'b')	// binary      e.g. 0b01100110
		raw = raw.substr(2);

	else									// decimal     e.g. 20114
		raw = dec_to_bin(raw);

	cout << "Output of bin conv is " + raw << endl;
	return state(raw, true);


}

void instruction::parse(string raw, map<string, keyword*> types, map<string, variable*> vars, string tab)
{
	result.clear();

	chp = raw;

	string::iterator i;
	unsigned long int j, k;
	int name_end;
	int assign_start;

	string temp;

	list<string> var;
	list<string>::iterator var_iter;
	list<state> val;
	list<state>::iterator val_iter;

	map<string, state>::iterator state_iter;

	// Parse assignment instructions
	// Currently only handles single variable assignments
	if (chp.find(":=") != chp.npos)
	{
		// Separate the two operands (the variable to be assigned and the value to assign)
		name_end = chp.find(":=");
		temp = chp.substr(0,name_end);
		for (j = temp.find_first_of(","), k = 0; j != temp.npos; j = temp.find_first_of(",", j+1))
		{
			var.push_back(temp.substr(k, j-k));
			k = j;
		}
		var.push_back(temp.substr(k));

		temp = chp.substr(name_end+2);
		for (j = temp.find_first_of(","), k = 0; j != temp.npos; j = temp.find_first_of(",", j+1))
		{
			val.push_back(expr_eval(temp.substr(k, j-k)));
			cout << temp.substr(k, j-k) << endl;
			k = j;
		}
		cout << "expr_eval called on " + temp.substr(k) << endl;
		val.push_back(expr_eval(temp.substr(k)));



		for (var_iter = var.begin(), val_iter = val.begin(); var_iter != var.end() && val_iter != val.end(); var_iter++, val_iter++)
			result.insert(pair<string, state>(*var_iter, *val_iter));

		cout << tab << "Instruction:\t"+chp << endl;
	}
	// Parse Communication instructions (send, receive, and probe)
	// Currently unsupported
	else if (chp.find_first_of("!?@") != chp.npos)
	{

	}
	// Parse skip
	else if (chp.find("skip") != chp.npos)
		return;
	// The I don't know block
	else
		cout << "\t\tError: Instruction not handled: "+chp << endl;

	cout << tab << "Result:\t";

	for (state_iter = result.begin(); state_iter != result.end(); state_iter++)
		cout << "{" << state_iter->first << " = " << state_iter->second << "} ";
	cout << endl;
}


