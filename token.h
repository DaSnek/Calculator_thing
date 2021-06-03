#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

enum {
	TOKENTYPE_NONE,
	TOKENTYPE_NUM,
	TOKENTYPE_PLUS,
	TOKENTYPE_MINUS,
	TOKENTYPE_MUL,
	TOKENTYPE_DIV,
	TOKENTYPE_POW,
	TOKENTYPE_EL,
	TOKENTYPE_RP,
	TOKENTYPE_LP
};

struct Token {
	int type;
	std::string value_s;
	double value_d;
	int offset;

	std::string to_string() const;
	
	Token() : type(TOKENTYPE_NONE), value_s(), value_d(0), offset(0) {}
	
//	Token(Token const& t) : type(t.type), value_s(t.value_s), value_d(t.value_d), offset(t.offset) {}
		
	Token(Token const& t) = default;

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

	void set_number(int off, double n){
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

class TokenParser {
	std::string src;
	int cur_offset;

public:
	explicit TokenParser(const std::string& line);
	Token get_token();
	virtual ~TokenParser();
	void print_error(const std::string& msg);
};

#endif
