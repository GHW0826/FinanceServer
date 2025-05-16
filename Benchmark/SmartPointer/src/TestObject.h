#pragma once
#include <string>
using namespace std;

class TestObject
{
public:
	TestObject(const char* name) 
		: _name(name)
	{
	}

	~TestObject() 
	{ 
		std::cout << "TestObject " << _name << " destroyed\n"; 
	}

	const char* getName() const { return _name.c_str(); }
private:
	string _name;
};