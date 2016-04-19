#pragma once

#include <vector>
#include <string>

using namespace std;

#define NO_ISSUE -1

class SuperScalar
{
	vector<vector<string>> InstQueue;
	vector<float> Memory;
	int m_iProgramCounter;
	int m_iCurrentIssue;
	int m_iPCMax;
	bool m_bExeComplete;
	//Todo: create a struct(Inst) containing index and remaining cycles
	//Todo: create a stl list of struct Inst for LoadStore functional unit
	//Todo: create a stl list of struct Inst for int functional unit
	//Todo: create a stl list of struct Inst for FP functional unit


public:
	SuperScalar() :Memory(1024), m_iProgramCounter(0), m_iCurrentIssue(NO_ISSUE), m_iPCMax(0)
	{}
	int ReadInputFile(string path);
	void Simulate();
	void AddInstruction(string line);
	void issue();
	//All todo
	//LoadStore()
	//ExecuteInt()
	//ExecuteFP()
};