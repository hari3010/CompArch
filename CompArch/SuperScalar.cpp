#include "stdafx.h"
#include "SuperScalar.h"

using namespace std;

//parse the line and add it to instruction queue
void SuperScalar::AddInstruction(string strLine)
{
	vector<string> vInst;
	string tempStr;
	size_t locateIndex;
	size_t CurrentIndex;

	//!< Push Instruction eg: FPMULT.
	locateIndex = 
		strLine.find(" ");
	if (string::npos != locateIndex)
	{
		// string before first space is instruction
		vInst.push_back(strLine.substr(0, locateIndex));
	}
	else
	{
		//Halt may have space or end line
		vInst.push_back(strLine);
	}

	CurrentIndex = locateIndex + 1;
	//!< Add first operand eg: R0
	locateIndex = strLine.find(",", CurrentIndex);
	if (string::npos != locateIndex)
	{
		//string from first space and first comma
		vInst.push_back(strLine.substr(CurrentIndex, locateIndex- CurrentIndex - 1));
	}

	CurrentIndex = locateIndex + 1;
	//!< Add first operand eg: R0
	locateIndex = strLine.find(",", CurrentIndex);
	if (string::npos != locateIndex)
	{
		//string from first space and first comma
		vInst.push_back(strLine.substr(CurrentIndex, locateIndex - CurrentIndex - 1));
	}

	CurrentIndex = locateIndex;
	//!< Add first operand eg: R0
	locateIndex = strLine.find(",", CurrentIndex);
	if (string::npos != locateIndex)
	{
		//string from first space and first comma
		vInst.push_back(strLine.substr(CurrentIndex + 1, locateIndex - CurrentIndex - 1));
	}


	InstQueue.push_back(vInst);
}
