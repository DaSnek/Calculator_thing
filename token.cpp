#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

#include "token.h"

using std::string;

extern bool debug;
extern double prev_result;

static bool is_digit(char c) {
	return c <= '9' && c >= '0';
}

static bool is_letter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c >- 'z');
}

//aids in printing and debugging
string Token::to_string() const {
	std::stringstream ss;

	switch (type) {
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

	case TOKENTYPE_NEG:
		ss << "<(0-1)>";
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

	case TOKENTYPE_NONE:
		ss << "<NONE>";
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

std::ostream& operator << (std::ostream& os, const Token& t) {
	os << t.to_string() << ' ';
	return os;
}

TokenParser::TokenParser(const std::string& line) : src(line), cur_offset(0), prev_token() {}

void TokenParser::print_error(const string& msg) const {
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

//cuts the string input by the user into individual "tokens"
Token TokenParser::get_token() {
	Token token;

	if (debug) 
		std::cout << "prev_t type: " << prev_token.to_string() << std::endl;

	//if the current position we are looking at is already the end, then we return an end of line token
	// which ranks lower than everything else
	if (cur_offset >= src.size()) {
		token.set_el();
		prev_token = token;
		return token;
	}

	//skips over the spaces
	while (cur_offset < src.size() && is_space(src[cur_offset])) 
		cur_offset++;

	//assigns types to values accordingly
	if (src[cur_offset] ==  '+')  {
		token.set_plus(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}	
	
	if (src[cur_offset] ==  '-')  {
		if (prev_token.type == TOKENTYPE_NUM || prev_token.type == TOKENTYPE_RP) {
			token.set_minus(cur_offset);	//special case for the '-' operator.
			cur_offset++;					//if the token in front of it is a number or a rp 
			prev_token = token;				//then its a operator -, otherwise its a negative sign				
			return token;					
		} else {
			token.set_neg(cur_offset);
			cur_offset++;
			prev_token = token;
			
			return token;
		}
	}	
	
	if (src[cur_offset] ==  '*')  {
		token.set_mul(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}	
	
	if (src[cur_offset] ==  '/')  {
		token.set_div(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}	

	if (src[cur_offset] == '(') {
		token.set_lp(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}

	if (src[cur_offset] == ')') {
		token.set_rp(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}

	if (src[cur_offset] == '^') {
		token.set_pow(cur_offset);
		cur_offset++;
		prev_token = token;
		return token;
	}

	// if we encounter a $ sign, then we set its type as a number and 
	//assigns the previous result to it
	if (src[cur_offset] == '$') {
		token.set_number(cur_offset, prev_result);
		cur_offset++;
		prev_token = token;
		return token;
	}

	//if we encounter a digit...
	if (is_digit(src[cur_offset])) {
		int end = cur_offset + 1;			// the location of the last digit of the number
	
		//iterating through the input until either we get to the end 
		//or encounter something that is not a digit
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
		prev_token = token;
		
		return token;
	}

	//if the token doesn't fit any of the cases, then its an error
	print_error("unknown token");
	throw string();
}

TokenParser::~TokenParser() {}
