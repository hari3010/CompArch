#include "stdafx.h"
#include "SuperScalar.h"

using namespace std;

void SuperScalar::Execute()
{
	//Todo
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
