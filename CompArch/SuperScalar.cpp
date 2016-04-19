#include "stdafx.h"
#include "SuperScalar.h"

using namespace std;

void SuperScalar::issue()
{
	//For the first time, nothing will be issued
	if (m_iCurrentIssue != NO_ISSUE)
	{
		//Todo
		//Register rename
		//Assign job to a specific execution queue
	}

	//First time comes here, issue first job
	if (m_iCurrentIssue == NO_ISSUE)
	{
		m_iCurrentIssue = m_iProgramCounter;

		if (m_iProgramCounter < m_iPCMax)
		{
			m_iProgramCounter++;
		}
	}
}

//Simulate the set of instruction and find the total number of cycles
void SuperScalar::Simulate()
{
	int iCycles = 0;
	m_bExeComplete = false;
	while (!m_bExeComplete)
	{
		issue();

		iCycles++;
	}
	cout << "Total number of cycles" << iCycles << endl;

}

//parse the line and add it to instruction queue
void SuperScalar::AddInstruction(string strLine)
{
	vector<string> vInst;
	string tempStr;
	
	size_t i;
	size_t CurrentIndex = 0;
	
	for (i = 0; i < strLine.length(); i++)
	{
		//iterate through all letters if it is space, comma or tab 
		//mark it as a demiliter. copy the sub strings from the current index to delimitor
		if (strLine[i] == ' ' || strLine[i] == ',' || strLine[i] == '\t')
		{
			if (i != CurrentIndex)
			{
				vInst.push_back(strLine.substr(CurrentIndex, i - CurrentIndex));
			}	
			CurrentIndex = i + 1;
		}
	}
	if (CurrentIndex != strLine.length())
	{
		vInst.push_back(strLine.substr(CurrentIndex));
	}

	InstQueue.push_back(vInst);
}

void SuperScalar::issue()
{
	throw std::logic_error("The method or operation is not implemented.");
}
