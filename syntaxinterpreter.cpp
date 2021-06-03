#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>
#include <string.h>
#include <cassert>

#include "token.h"
#include "stack.h"

using std::string;

class ExpresParser {
public:
	Stack<Token> operator_s;
	Stack<Token> value_s;

public:
	ExpresParser() : operator_s(), value_s() {}
	
	int get_rank(Token const& t) {
		switch (t.type) {
		case TOKENTYPE_EL:
			return 0;
		
		case TOKENTYPE_RP:
			return 10;

		case TOKENTYPE_PLUS:
		case TOKENTYPE_MINUS:
			return 20;

		case TOKENTYPE_MUL:
		case TOKENTYPE_DIV:
			return 30;

		case TOKENTYPE_POW:
	//	case TOKENTYPE_SQRT:
			return 40;

		case TOKENTYPE_LP:
			return 50;
		
		default:
			assert(0);
		}
	}
	
	// return 1 if left > right, 
	bool compare_rank(Token const& left, Token const& right) {
		return get_rank(left) > get_rank(right);
	}
};

void top_calculate(ExpresParser& ep) {
	double result;
	if (ep.value_s.size() < 2) {
		std::cout << "Error: missing value, stack size " << ep.value_s.size() << std::endl;
		exit(-1);
	}

	Token op = ep.operator_s.pop();
	Token v1 = ep.value_s.pop();
	Token v2 = ep.value_s.pop();

	switch (op.type) {
	case TOKENTYPE_PLUS:
		result = v1.value_d + v2.value_d;
		break;

	case TOKENTYPE_MINUS:
		result = v2.value_d - v1.value_d;
		break;

	case TOKENTYPE_MUL:
		result = v2.value_d * v1.value_d;
		break;

	case TOKENTYPE_DIV:
		if (v1.value_d == 0) {
			std::cout << "Error: divide by 0 error" << std::endl;
			exit(-1);
		}
		result = v2.value_d / v1.value_d;
		break;

	case TOKENTYPE_POW:
		result = pow(v2.value_d, v1.value_d);
		break;
	}
	
	Token r;
	r.set_number(0, result);
	ep.value_s.push(r);
}

void print_result(ExpresParser& ep) {
	if (ep.value_s.size() != 1) {
		std::cout << "Error: missing operator(s)" << std::endl;
		exit(1);
	}
	std::cout << ep.value_s.pop().value_d << std::endl;
}

void process(ExpresParser& ep, Token const& t) {
	if (t.type == TOKENTYPE_NUM) {
		ep.value_s.push(t);
		return;
	}
	
	while (1) {
		if (t.type == TOKENTYPE_RP) {
			if (ep.operator_s.empty()) {
				printf("Error: no match for ')'\n");
				exit(-1);
			}

			Token top = ep.operator_s.peek();

			if (top.type == TOKENTYPE_LP) {
				ep.operator_s.pop();
				break;
			}		
		
			top_calculate(ep);
			continue;
		}	
	
		if (ep.operator_s.empty()) {
			if (t.type == TOKENTYPE_EL) {
				print_result(ep);
				return;
			}
			ep.operator_s.push(t);
			break;
		}

		Token top = ep.operator_s.peek();

		if (top.type == TOKENTYPE_LP) {
			ep.operator_s.push(t);
			break;
		}

		if (ep.compare_rank(t, top)) {
			ep.operator_s.push(t);
			break;
		}
		top_calculate(ep);	
	}	
}

int debug = 0;

int main(int ac, char*av[]) {
	string line;
	ExpresParser ep;

	for (int i = 1; i < ac; i++) {
		if (strcmp(av[i], "-d") == 0) {
			debug = 1;
		}
	}

	while (1) {
		printf("> ");
		std::getline(std::cin, line);
	//	printf("[%s]\n", line.c_str());
		if (line == "quit"|| line == "q"|| line == "exit")
			exit(0);
		if (line == "") {
			continue;
		}	
		
		TokenParser tp(line);
		Token t;

		do {
			t = tp.get_token();
			if (debug) 
				printf("%s\n",t.to_string().c_str());
			
			process(ep, t);
		} while (!t.is_el());
	}
}
