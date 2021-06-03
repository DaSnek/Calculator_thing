#include <stdio.h>
#include <string>
#include <sstream>

#include "token.h"

using std::string;

static bool is_digit(char c) {
	return c <= '9' && c >= '0';
}

string Token::to_string() const {
	std::stringstream ss;

	switch(type) {
	case TOKENTYPE_EL:
		ss << "<EOL>";
		break;

	case TOKENTYPE_LP:
		ss << "<(>";
		break;

	case TOKENTYPE_RP:
		ss << "<)>";
		break;

	case TOKENTYPE_PLUS:
		ss << "<+>";
		break;

	case TOKENTYPE_MINUS:
		ss << "<->";
		break;

	case TOKENTYPE_MUL:
		ss << "<*>";
		break;

	case TOKENTYPE_DIV:
		ss << "</>";
		break;

	case TOKENTYPE_POW:
		ss << "<^>";
		break;

	case TOKENTYPE_NUM:
		ss << "<NUMBER: " << value_d << ">";
		break;

	default:
		printf("unkown internal error\n");
		exit(-2);
	}
	return ss.str();
}

TokenParser::TokenParser(const std::string& line) : src(line), cur_offset(0) {}

void TokenParser::print_error(const string& msg) {
	printf("Error: %s\n", msg.c_str());
	printf("%s\n", src.c_str());
	for (int i = 0; i < cur_offset; i++) {
		printf(" ");
	}
	printf("^\n");
}

static bool is_space(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

Token TokenParser::get_token() {
	Token token;

	if (cur_offset >= src.size()) {
		token.set_el();
		return token;
	}

	while (cur_offset < src.size() && is_space(src[cur_offset])) 
		cur_offset++;

	if (src[cur_offset] ==  '+')  {
		token.set_plus(cur_offset);
		cur_offset++;
		return token;
	}	
	
	if (src[cur_offset] ==  '-')  {
		token.set_minus(cur_offset);
		cur_offset++;
		return token;
	}	
	
	if (src[cur_offset] ==  '*')  {
		token.set_mul(cur_offset);
		cur_offset++;
		return token;
	}	
	
	if (src[cur_offset] ==  '/')  {
		token.set_div(cur_offset);
		cur_offset++;
		return token;
	}	

	if (src[cur_offset] == '(') {
		token.set_lp(cur_offset);
		cur_offset++;
		return token;
	}

	if (src[cur_offset] == ')') {
		token.set_rp(cur_offset);
		cur_offset++;
		return token;
	}

	if (src[cur_offset] == '^') {
		token.set_pow(cur_offset);
		cur_offset++;
		return token;
	}

	if (is_digit(src[cur_offset])) {
		int end = cur_offset + 1;
	
		while (end < src.size() && (is_digit(src[end]) || src[end] == '.')) {
			end++;
		}
		
		size_t loc = 0;
		string s = src.substr(cur_offset, end - cur_offset);
		double d = std::stod(s, &loc);
		
		if (loc != s.size()) {
			print_error("invalid value");
			exit(-1);
		}

		token.set_number(cur_offset, d);
		cur_offset = end;
		
		return token;
	}
	print_error("unknown token");
	exit(1);
}

TokenParser::~TokenParser() {}