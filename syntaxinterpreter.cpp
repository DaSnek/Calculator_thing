#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>
#include <string.h>
#include <map>
#include <cassert>

#include "token.h"
#include "stack.h"

using std::string;

bool debug = false;
double prev_result = 0;
std::map<std::string, double> vars;

class ExpresParser {
public:
	Stack<Token> operator_s;
	Stack<Token> value_s;

public:
	ExpresParser() : operator_s(), value_s() {}
	
	//each operator has a rank, this is important as 
	//we have to compare ranks to decide if we should 
	//compute the last operation or not
	int get_rank(Token const& t) {
		switch (t.type) {
		case TOKENTYPE_EL:
			return 0;
		
		case TOKENTYPE_EQS: //
			return 10;

		case TOKENTYPE_RP:
			return 20;

		case TOKENTYPE_PLUS:
		case TOKENTYPE_MINUS:
			return 30;

		case TOKENTYPE_MUL:
		case TOKENTYPE_DIV:
			return 40;

		case TOKENTYPE_POW:
			return 50;

		case TOKENTYPE_NEG:
			return 60;

		case TOKENTYPE_FAC:
			return 70;

		case TOKENTYPE_LP:
			return 80;
		
		default:
			std::cout << "Unknown internal error" << std::endl;
			std::cout << "Exiting..." << std::endl;
			assert(0);
		}
	}
	
	// return 1 if left > right, 
	bool compare_rank(Token const& left, Token const& right) {
		if (left.type == right.type && 
				(left.type == TOKENTYPE_EQS || left.type == TOKENTYPE_POW)) {
			return true;
		}
		return get_rank(left) > get_rank(right);
	}
};

inline static Token get_token_value(const Token& t) {
	if (t.type == TOKENTYPE_NUM) {
		return t;
	}

	assert(t.type == TOKENTYPE_VAR);

	Token res;
	auto i = vars.find(t.value_s);
	if (i == vars.end()) {
		return res;
	}
		
	res.set_number(t.offset, i->second);
	return res;
}	

/*
inline static double get_var_value(std::string var_name) {
	return vars.find(var_name);
}
*/

//calculate the operation on the top most of the operator stack acorrdingly
void top_calculate(ExpresParser& ep) {
	double result;

	if (ep.value_s.size() < 2) {
		//if we have less than two values and an operator, then there
		//is a missing value. but the exception to this is the negative sign and factorials,
		//as they only have an effect on one number
		if (ep.operator_s.peek().type != TOKENTYPE_NEG && ep.operator_s.peek().type != TOKENTYPE_FAC) {
			std::cout << "Error: missing value, stack size " << ep.value_s.size() << std::endl;
			throw string();
		}
	}

	//we get the top most operator
	Token op = ep.operator_s.pop();
	
	//since the negative sign is a special case,
	//we process it seperatly.
	if (op.type == TOKENTYPE_NEG) {
		Token v = ep.value_s.pop();
		Token vn = get_token_value(v); 
		if (vn.type == TOKENTYPE_NONE) {
			std::cout << "Operations cannot be performed on uninitialized variables" << std::endl;
			throw string();
		}
		vn.set_number(0, v.value_d * -1);
	
		ep.value_s.push(vn);	
		return;
	}

	if (op.type == TOKENTYPE_FAC) {
		Token v = ep.value_s.pop();
		int res = 1;
		for (int i = 2; i <= v.value_d; i++) {
			res *= i;
		}
		v.set_number(0, res);
		ep.value_s.push(v);
		return;
	}

	//the two values we are processing
	Token v1 = ep.value_s.pop();
	Token v2 = ep.value_s.pop();

	Token v1n = get_token_value(v1);
	Token v2n = get_token_value(v2);

	if (op.type == TOKENTYPE_EQS) {
		if (v2.type == TOKENTYPE_NONE || v2.type == TOKENTYPE_NUM) {
			std::cout << "values cannot be assigned to non-variables" << std::endl;
			throw string();
		}
	//	std::cout << "=======" << std::endl;
		vars[v2.value_s] = v1n.value_d;
	//	std::cout << v2.value_s << "=" << v1n.value_d;
	//	std::cout << "{" << vars[v2.value_s] << "}" << std::endl;
		ep.value_s.push(v2);
		return;
	}

	if (v1n.type == TOKENTYPE_NONE) {
		std::cout << v1.value_s << ": uninitialized variable" << std::endl;
		throw string();
	} 

	if (v2n.type == TOKENTYPE_NONE) {
		std::cout << v2.value_s << ": uninitialized variable" << std::endl;
		throw string();
	} 

	switch (op.type) {
	case TOKENTYPE_PLUS:
		result = v1n.value_d + v2n.value_d;
		break;

	case TOKENTYPE_MINUS:
		result = v2n.value_d - v1n.value_d;
		break;

	case TOKENTYPE_MUL:
		result = v2n.value_d * v1n.value_d;
		break;

	case TOKENTYPE_DIV:
		if (v1n.value_d == 0) {				//I don't think this one needs explaining...
			std::cout << "Error: divide by 0 error" << std::endl;
			throw string("");
		}
		result = v2n.value_d / v1n.value_d;
		break;

	case TOKENTYPE_POW:
		result = pow(v2n.value_d, v1n.value_d);
		break;
	}
	
	Token r;
	r.set_number(0, result);
	ep.value_s.push(r);		//then we put the result back into the numbers stack
}

//prints our result and saves it as the previous result, for the next calculation.
void print_result(ExpresParser& ep) {
	if (ep.value_s.size() != 1) {
		std::cout << "Error: missing operator(s)" << std::endl;
		throw string("");
	} 

	Token t = ep.value_s.pop();

	if (t.type == TOKENTYPE_VAR) {
		Token tn = get_token_value(t);
		if (tn.type == TOKENTYPE_NONE) {
			std::cout << t.value_s << ": uninitialied variable" << std::endl;
			throw string();
		}
		std::cout << tn.value_d << std::endl;
		prev_result = tn.value_d;
	} else {
		std::cout << t.value_d << std::endl;
		prev_result = t.value_d;
	}
}

//debugging uses
static void dump_vars() {
	std::cout << "--------------vars--------------" << std::endl;
	for (auto& p : vars) {
		std::cout << p.first << " : " << p.second << std::endl;
	}
	std::cout << "--------------------------------" << std::endl << std::endl;
}

// I can't really be bothered to explain this part, but basicly it 
//processes each token and decides if the current token should be put into the stacks
//according to the ranks, and carries out calculations if needed 
void process(ExpresParser& ep, Token const& t) {
	if (debug) {
		std::cout << "--------values-------------" << std::endl;
		ep.value_s.dump_data();
		std::cout << "----------op---------------" << std::endl;
		ep.operator_s.dump_data();
		std::cout << "----------------------------" << std::endl;
		std::cout << std::endl << std::endl;
		dump_vars();
	}
	//if the token is numberor a variable, then it goes into the values stack automaticly
	if (t.type == TOKENTYPE_NUM || t.type == TOKENTYPE_VAR) {
		ep.value_s.push(t);
		return;
	}
	
	while (1) {
		// if we encounter a ')', then we have to calculate everything until the
		//'(', and if we don't encounter a '(', we consider that an error 
		if (t.type == TOKENTYPE_RP) {
			if (ep.operator_s.is_empty()) {
				printf("Error: no match for ')'\n");
				throw string();
			}

			Token top = ep.operator_s.peek();

			//when we get to the left parentasis we can just throw it away
			//and break out of the loop
			if (top.type == TOKENTYPE_LP) {
				ep.operator_s.pop();
				break;
			}		
		
			top_calculate(ep);
			continue;
		}	
	
		if (ep.operator_s.is_empty()) {
			//if in the operator stack there is only the EOL token left,
			//then it means we can retreve the result
			if (t.type == TOKENTYPE_EL) {
				print_result(ep);
				return;
			}
			ep.operator_s.push(t);
			break;
		}

		Token top = ep.operator_s.peek();

		//we can keep putting stuff ontop of the '(', even though
		//it has the highest rank.
		if (top.type == TOKENTYPE_LP) {
			ep.operator_s.push(t);
			break;
		}
	
		//otherwise we just compare the most recent operator and 
		//the current operator's ranks
		if (ep.compare_rank(t, top)) {
			ep.operator_s.push(t);		//if the current op's rank is higher. 
			break;						//then we can put it into the stack and break
		}
		top_calculate(ep);				//otherwise we have to calculate the top
	}									//operation in the stack first and 
										//compare the current op to the next top
}										//in the stack 

void print_usage(bool init, bool more) {
	if (!more) {
		if (init) {
			printf("\n");
			printf("CLC (Command Line Calculator) Version 21.3.1 (stable)\n\n");

			printf(debug ? "Debug mode on \n\n" : "");

			printf("Small tips: ");
			printf("CLC currently does not support root operations, please use fraction exponential forms as substitutes.\n");
			printf("This issue will be fixed as soon as possible on the next major update to support functions. Sorry for the inconvinience.\n\n");
		}
		printf("Use '$' for the previous result. (Ex: 402 + $ ^ 2)\n");
		printf("This calculator also supports variables, (remember the names are english letters or '_' only). You can use 'vars' to see all variables you have\n");
		printf("Type 'quit, 'q', or 'exit' to quit.\n\n");

		printf("Type 'help' to see this message again, and 'info' for more information\n");
		return;
	}
	printf("There is a debug mode, use -d as an argument when excuting the program to activate it.\n");
	printf("Github link for the project repository: https://github.com/DaSnek/Calculator_thing.\n");
	printf("Please leave any issues and suggestions on my github page for this program, they are very much appreciated. :)\n");
}

int main(int ac, char*av[]) {
	std::string line;

	for (int i = 1; i < ac; i++) {
		if (strcmp(av[i], "-d") == 0) {
			debug = true;
		}
	}
	
	print_usage(true, false);

	// the reason for the usage of exceptions is that 
	//if the user makes an mistake and the program encounters
	//error, then the program could just ignore it and move on
	//instead of crashing
	while (1) {
		try {
			ExpresParser ep;
			printf("> ");
			std::getline(std::cin, line);

			if (line == "") 
				continue;

			if (line == "quit"|| line == "q"|| line == "exit") {
				printf("(exited)\n\n");				
				exit(0);
			}

			if (line == "help") {
				print_usage(false, false);
				continue;
			}
			
			if (line == "info") {
				print_usage(false, true);
				continue;
			}

			if (line == "vars") {
				dump_vars();
				continue;
			}

			TokenParser tp(line);
			Token t;

			do {
				t = tp.get_token();	// get a token from the string
				if (debug) 
					printf("%s\n",t.to_string().c_str());
				process(ep, t); 	//process each token after we get them
			} while (!t.is_el());	// and stop after we get to EOL
		} catch (std::string s) {
			continue;
		}
	}
}
