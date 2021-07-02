#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

//all of the types of tokens we can have
enum {
	TOKENTYPE_NONE,
	TOKENTYPE_NUM,
	TOKENTYPE_PLUS,
	TOKENTYPE_MINUS,
	TOKENTYPE_MUL,
	TOKENTYPE_DIV,
	TOKENTYPE_POW,
	TOKENTYPE_NEG,
	TOKENTYPE_FAC, //
	TOKENTYPE_EL,
	TOKENTYPE_RP,
	TOKENTYPE_LP,
};

struct Token {
	int type;				//the token's type
	std::string value_s;	//each token one of two values, either a string value,
	double value_d;			//or a number value
	int offset;				//not nessary, for good for printing out errors

	std::string to_string() const;
	
	Token() : type(TOKENTYPE_NONE), value_s(), value_d(0), offset(0) {}
	
//	Token(Token const& t) : type(t.type), value_s(t.value_s), value_d(t.value_d), offset(t.offset) {}
		
	Token(Token const& t) = default;

	//helpers to inicialize the tokens
	//and sets the token types
	void set_el() {
		type = TOKENTYPE_EL;
	}
	
	bool is_el() const { 
		return type == TOKENTYPE_EL;
	}

	void set_plus(int off) {
		type = TOKENTYPE_PLUS;
		value_s = "+";
		offset = off;
	}

	void set_minus(int off) {
		type = TOKENTYPE_MINUS;
		value_s = "-";
		offset = off;
	}

	void set_neg(int off) {
		type = TOKENTYPE_NEG;
		value_s = "(0-1)";
		offset = off;
	}

	void set_mul(int off) {
		type = TOKENTYPE_MUL;
		value_s = "*";
		offset = off;
	}

	void set_div(int off) {
		type = TOKENTYPE_DIV;
		value_s = "/";
		offset = off;
	}

	void set_pow(int off) {
		type = TOKENTYPE_POW;
		value_s = "^";
		offset = off;
	}

	void set_fac(int off) {  //
		type = TOKENTYPE_FAC;
		value_s = "!";
		offset = off;
	}

	void set_number(int off, double n) {
		type = TOKENTYPE_NUM;
		offset = off;
		value_d = n;
	}

	void set_lp(int off) {
		type = TOKENTYPE_LP;
		value_s = "(";
		offset = off;
	}

	void set_rp(int off) {
		type = TOKENTYPE_RP;
		value_s = ")";
		offset = off;
	}

	~Token() {}
};

//printing the tokens, for debugging purposes
std::ostream& operator << (std::ostream& os, const Token& t);

class TokenParser {
	std::string src;
	int cur_offset;

public:
	explicit TokenParser(const std::string& line);
	Token get_token();
	virtual ~TokenParser();
	void print_error(const std::string& msg) const;

	Token prev_token;
};

#endif
