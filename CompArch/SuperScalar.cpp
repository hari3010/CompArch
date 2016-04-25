#include "stdafx.h"
#include "SuperScalar.h"

using namespace std;


// Issue the current instruction to the Execution Queue
void SuperScalar::IssueToFU()
{
	switch (InstQueue[m_iCurrentIssue].eMne)
	{
	case E_LOAD:
	case E_MOV:
	{	stROB l_rob(InstQueue[m_iCurrentIssue], 2, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iLoadStoreFU++;
		break;
	}
	case E_STR:
	{
		stROB l_rob(InstQueue[m_iCurrentIssue], 4, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iLoadStoreFU++;
		break;
	}
	case E_FPADD:
	case E_FPSUB:
	{
		stROB l_rob(InstQueue[m_iCurrentIssue], 4, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iFPAdderFU++;
		break;
	}
	case E_FPMULT:
	{
		stROB l_rob(InstQueue[m_iCurrentIssue], 6, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iFPMultiplierFU++;
		break;
	}
	case E_FPDIV:
	{
		stROB l_rob(InstQueue[m_iCurrentIssue], 7, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iFPMultiplierFU++;
		break;
	}
	case E_ADD:
	case E_SUB:
	{
		stROB l_rob(InstQueue[m_iCurrentIssue], 2, true, m_iCurrentIssue);
		listROB.push_back(l_rob);
		m_iIntFU++;
		break;
	}
	default:
		break;
	}
}


void SuperScalar::AddMemory(string strLine)
{
	strLine = strLine.substr(strLine.find('<') + 1);
	stringstream ss1(strLine);
	int index;
	ss1 >> index;
	strLine = strLine.substr(strLine.find('<') + 1);
	stringstream ss2(strLine);
	ss2 >> Memory[index];
}

int SuperScalar::issue()
{
	//For the first time, nothing will be issued
	if (m_iCurrentIssue != NO_ISSUE)
	{
		//If any instruction issued in previous instruction 
		IssueToFU();
		m_iCurrentIssue = NO_ISSUE;
		//Register rename
		//Assign job to a specific execution queue
	}

	//First time comes here, issue first job
	if (m_iCurrentIssue == NO_ISSUE && FU_AVAILABLE == CheckFU(m_iProgramCounter))
	{
		m_iCurrentIssue = m_iProgramCounter;

		PRINT("Current Issued::" << m_iCurrentIssue << "Cycle::" << m_iCycles);

		if (m_iProgramCounter < m_iPCMax)
		{
			m_iProgramCounter++;
		}
	}

	return 0;
}


int SuperScalar::Execute()
{
	for (std::list<stROB>::iterator itROB = listROB.begin(); itROB != listROB.end(); )
	{
		bool bRemoveCurrentValue = false;

		switch (itROB->Inst.eMne)
		{
		case E_FPDIV:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 / itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iFPMultiplierFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << " at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << " at Cycle" << m_iCycles);
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_ADD:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 + itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iIntFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles );
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_SUB:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}


				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 - itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iIntFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles );
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_FPADD:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 + itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iFPAdderFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles );
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_FPSUB:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 - itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iFPAdderFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles);
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_FPMULT:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}
				
				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src1 * itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iFPMultiplierFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << " at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << " at Cycle" << m_iCycles );
					itROB = listROB.erase(itROB);

				}
			}
			break;
		}
		case E_LOAD:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				
				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
						eRegStatus = STATUS_FREE;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
					eRegStatus = STATUS_FREE;
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
					eRegStatus = STATUS_FREE;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[(int)itROB->Inst.op1].value = Memory[(int)itROB->src2];
					Register[(int)itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iLoadStoreFU--; 
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles );
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_MOV:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
						eRegStatus = STATUS_FREE;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
					eRegStatus = STATUS_FREE;
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
					eRegStatus = STATUS_FREE;
				}

				if (eRegStatus == STATUS_FREE)
				{
					itROB->hazard = false;
					Register[itROB->Inst.op1].eStatus = STATUS_BUSY;
				}

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Register[itROB->Inst.op1].value = itROB->src2;
					Register[itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iLoadStoreFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles);
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		case E_STR:
		{
			if (itROB->hazard == true)
			{
				E_BUSY_STATUS eRegStatus = STATUS_BUSY;
				//Check the second operand register is free
				if (Register[itROB->Inst.op2].eStatus == STATUS_FREE)
				{
					eRegStatus = STATUS_FREE;
					itROB->src1 = Register[itROB->Inst.op2].value;
					itROB->bsrc1Avail = true;
				}

				//Check if thrid operand is register, if it is, then check it is free
				if (itROB->Inst.select == REGISTER)
				{
					if (Register[(int)itROB->Inst.op3].eStatus == STATUS_FREE)
					{
						itROB->src2 = Register[(int)itROB->Inst.op3].value;
						itROB->bsrc2Avail = true;
					}
					else
					{
						eRegStatus = STATUS_BUSY;
					}
				}
				else if (itROB->Inst.select == MEMORY)
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = Memory[(int)itROB->Inst.op3];
				}
				else
				{
					itROB->bsrc2Avail = true;
					itROB->src2 = itROB->Inst.op3;
				}

				if (eRegStatus == STATUS_FREE)
					itROB->hazard = false;

				//If value taken from CDB itROB->remainingCycles--;


			}
			else
			{
				itROB->remainingCycles--;
				if (itROB->remainingCycles == 0)
				{
					Memory[(int)itROB->src2] = itROB->src1;
					Register[(int)itROB->Inst.op2].eStatus = STATUS_FREE;
					m_iLoadStoreFU--;
					bRemoveCurrentValue = true;
					PRINT("Execution Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles-1);
					PRINT("Write Finished for :: " << itROB->PCindex << "at Cycle" << m_iCycles);
					itROB = listROB.erase(itROB);
				}
			}
			break;
		}
		default:
			break;
		}

		//!< If nothing deleted, move to the next iterator
		if (!bRemoveCurrentValue)
		{
			++itROB;
		}
	}

	return 0;
}

//Check if the Functional unit is busy for the given instruction
//input: iIndex - Inst Queue index
//out: Error if invalid input, O if functional unit free, 1 if FU not free
int SuperScalar::CheckFU(int iPCIndex)
{
	int iRet = FU_AVAILABLE;

	//If the index is less than the size of the buffer, then the input is wrong
	if (iPCIndex < (int)InstQueue.size())
	{
		switch (InstQueue[iPCIndex].eMne)
		{
		case E_LOAD:
		case E_MOV:
		case E_STR:
			if (!(m_iLoadStoreFU < FU_MAXSIZE_LOADSTORE))
				iRet = FU_BUSY;
			break;
		case E_FPADD:
		case E_FPSUB:
			if (!(m_iFPAdderFU < FU_MAXSIZE_FPADDSUB))
				iRet = FU_BUSY;
			break;
		case E_FPMULT:
		case E_FPDIV:
			if (!(m_iFPMultiplierFU < FU_MAXSIZE_FPMULDIV))
				iRet = FU_BUSY;
			break;
		case E_ADD:
		case E_SUB:
			if (!(m_iIntFU < FU_MAXSIZE_INT))
				iRet = FU_BUSY;
			break;
		default:
			break;
		}
	}
	else
	{		
		iRet = ERROR;
	}
	
	return iRet;
}

//Simulate the set of instruction and find the total number of cycles
void SuperScalar::Simulate()
{
	m_iCycles = 1;
	m_bExeComplete = false;
	while (!m_bExeComplete)
	{
		issue();

		Execute();

		m_iCycles++;
	}
	cout << "Total number of cycles" << m_iCycles << endl;

}

//parse the line and add it to instruction queue
void SuperScalar::AddInstruction(string strLine)
{
	vector<string> vInst;
	string tempStr;
	
	size_t i;
	size_t CurrentIndex = 0;
	
	//Parse all the operand in string format
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

	//One instuction structure
	Instruction l_Inst;

	//Check mnemonics
	if (vInst[0] == "FPADD")
	{
		l_Inst.eMne = E_FPADD;
	}
	else if (vInst[0] == "FPSUB")
	{
		l_Inst.eMne = E_FPSUB;
	}
	else if (vInst[0] == "FPMULT")
	{
		l_Inst.eMne = E_FPMULT;
	}
	else if (vInst[0] == "FPDIV")
	{
		l_Inst.eMne = E_FPDIV;
	}
	else if (vInst[0] == "ADD")
	{
		l_Inst.eMne = E_ADD;
	}
	else if (vInst[0] == "SUB")
	{
		l_Inst.eMne = E_SUB;
	}
	else if (vInst[0] == "LOAD")
	{
		l_Inst.eMne = E_LOAD;
	}
	else if (vInst[0] == "MOV")
	{
		l_Inst.eMne = E_MOV;
	}
	else if (vInst[0] == "STR")
	{
		l_Inst.eMne = E_STR;
	}
	else if (vInst[0] == "BR")
	{
		l_Inst.eMne = E_BR;
	}
	else if (vInst[0] == "BGT")
	{
		l_Inst.eMne = E_BGT;
	}
	else if (vInst[0] == "BLT")
	{
		l_Inst.eMne = E_BLT;
	}
	else if (vInst[0] == "BGE")
	{
		l_Inst.eMne = E_BGE;
	}
	else if (vInst[0] == "BLE")
	{
		l_Inst.eMne = E_BLE;
	}
	else if (vInst[0] == "BZ")
	{
		l_Inst.eMne = E_BZ;
	}
	else if (vInst[0] == "BNEZ")
	{
		l_Inst.eMne = E_BNEZ;
	}
	else if (vInst[0] == "HALT")
	{
		l_Inst.eMne = E_HALT;
	}

	size_t isize = vInst.size();
	switch (l_Inst.eMne)
	{
		case E_BR:
		{
			stringstream ss;
			if (vInst[1][0] == '#')
			{
				ss.str(vInst[1].substr(1));
				l_Inst.select = NUMBER;
				PRINT(ss.str());
			}
			else if (vInst[1][0] == 'R')
			{
				ss.str(vInst[1].substr(1));
				l_Inst.select = REGISTER;
			}
			else
			{
				ss.str(vInst[1]);
				l_Inst.select = MEMORY;
			}
			ss >> l_Inst.op3;
			break;
		}
		case E_FPADD:
		case E_FPSUB:
		case E_FPMULT:
		case E_FPDIV:
		case E_ADD:
		case E_SUB:
		{
			//Operand1
			stringstream ss1(vInst[1].substr(1));
			ss1 >> l_Inst.op1;

			//Operand2
			stringstream ss2(vInst[2].substr(1));
			ss2 >> l_Inst.op2;

			//Operand3
			stringstream ss;
			if (vInst[3][0] == '#')
			{
				ss.str(vInst[3].substr(1));
				l_Inst.select = NUMBER;
				PRINT(ss.str());
			}
			else if (vInst[3][0] == 'R')
			{
				ss.str(vInst[3].substr(1));
				l_Inst.select = REGISTER;
			}
			else
			{
				ss.str(vInst[3]);
				l_Inst.select = MEMORY;
			}
			ss >> l_Inst.op3;
			break;
		}
		case E_MOV:
		case E_LOAD:
		case E_STR:
		case E_BGT:
		case E_BLT:
		case E_BLE:
		case E_BGE:
		case E_BNEZ:
		case E_BZ:
		{	
			//Operand2
			stringstream ss2(vInst[1].substr(1));
			ss2 >> l_Inst.op2;

			//Operand3
			stringstream ss;
			if (vInst[2][0] == '#')
			{
				ss.str(vInst[2].substr(1));
				l_Inst.select = NUMBER;
				PRINT(ss.str());
			}
			else if (vInst[2][0] == 'R')
			{
				ss.str(vInst[2].substr(1));
				l_Inst.select = REGISTER;
			}
			else
			{
				ss.str(vInst[2]);
				l_Inst.select = MEMORY;
			}
			ss >> l_Inst.op3;
			break;
		}
		default:
			break;
	}

	InstQueue.push_back(l_Inst);
}
