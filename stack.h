#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cassert>

template <typename T>
class Stack {
private:
	std::vector<T> vec;
	
public:
	Stack() : vec() {}

	void push(const T& t) {
		vec.push_back(t);
	}

	T pop() {
		assert(vec.size() != 0);
		
		T t = vec.back();
		vec.pop_back();
		return t;
	}

	bool empty() {
		return vec.size() == 0;
	}

	int size() {
		return vec.size();
	}

	T const& peek() {
		return vec.back();
	}
};

#endif
