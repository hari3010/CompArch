#pragma once

#include <vector>
#include <string>

using namespace std;

class SuperScalar
{
	vector<vector<string>> InstQueue;
	vector<float> Memory;

public:
	SuperScalar():Memory(1024){}
	int ReadInputFile(string path);
	void AddInstruction(string line);
};