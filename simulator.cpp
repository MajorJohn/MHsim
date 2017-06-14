#include "simulator.h"

using namespace std;

MHsim::MHsim ()
{
	//read the "config" document
	int i = 0;

	string line;
	ifstream myfile ("config.txt");

	if (myfile.is_open())
	{
		while(!myfile.eof())
		{
			getline(myfile, line);

			transNum(i, line);

			i++;
		}

		//printConfig();

		// save/att parameters
		_iSizeBLock = _viConfLines[0];
		_iNLines = _viConfLines[1];
		_iNBlock = _viConfLines[2];

		_iCashSize = _iNLines*_iSizeBLock;
		_iMemorySize = _iNBlock*_iSizeBLock;

		_mCache = new stLine[_iCashSize];
		_vMemory = new stLine[_iMemorySize];

		//Partially Associative
		_iNWays = _viConfLines[4];
		_iWaySize = _iNLines / _iNWays;

		//Replacement Policy
		_viReplacement = new int[_iNLines];

		for (int i = 0; i < _iNLines; ++i)
		{
			_viReplacement[i] = 0;
		}
		

		//put invalid numbers in the cash.block
		//the rest could be trash
		for (int i = 0; i < _iCashSize; ++i)
		{
			_mCache[i].block = -1;
			_mCache[i].adress = -1;
		}

		//put the rights values in the memory.block and memory.adress
		//the memory.values could be trash
		for (int i = 0; i < _iMemorySize; ++i)
		{
			_vMemory[i].block = i/_iSizeBLock;
			_vMemory[i].adress = i;
		}

		//open input.txt
		myfile.close();
		myfile.open ("input.txt");

		_iNComands = 0;
		while(!myfile.eof())
		{
			getline(myfile, line);
			_iNComands++;
		}

		_sComands = new string[_iNComands];

		//reopen file
		myfile.close();
		myfile.open ("input.txt");

		i = 0;
		while(!myfile.eof())
		{
			getline(myfile, line);

			_sComands[i] = line;

			i++;
		}
	}
}

MHsim::~MHsim()
{
	delete [] _mCache;
	delete [] _vMemory;
	delete [] _sComands;
	delete [] _viReplacement;
}

// ***************************************
// **********  DECODE  *******************
// ***************************************

void //DONE
MHsim::transNum(int i, string line)
{
	_viConfLines[i] = 0;
	for (unsigned int j = 0; j < line.size(); ++j)
	{
		_viConfLines[i] = _viConfLines[i]*10;
		_viConfLines[i] += line[j] - 48;
	}
}

// ***************************************
// **********  DEBUG  ********************
// ***************************************

void //DONE
MHsim::printConfig()
{
	cout << ">>> config.txt \n\n";

	for (int i = 0; i < 7; ++i)
	{
		cout << _viConfLines[i] << endl;
	}

	cout << "\n>>> end config.txt \n\n";
}

void //DONE
MHsim::printComands()
{
	cout << ">>> input.txt \n\n";
	for (int i = 0; i < _iNComands; ++i)
	{
		cout <<_sComands[i] << endl;
	}
	cout << "\n>>> end input.txt \n\n";
}

// ***************************************
// **********  CONFIG SIMULATOR  *********
// ***************************************

void //DONE
MHsim::simulate()
{
	for (int i = 0; i < _iNComands; ++i)
	{
		//cout <<_sComands[i] << endl;
		for (unsigned int j = 0; j < _sComands[i].size(); ++j)
		{
			if(_sComands[i][j] == 'R' || _sComands[i][j] == 'r')
			{
				//cout << "\n>>> Read calles \n";
				preRead(i);
				break;
			}
			else if(_sComands[i][j] == 'W' || _sComands[i][j] == 'w')
			{
				//cout << "\n>>> Write calles \n";
				preWrite(i);
				break;
			}
			else if(_sComands[i][j] == 'S' || _sComands[i][j] == 's')
			{
				simShow();
				break;
			}
		}
	}
}

// ***************************************
// **********  READ  *********************
// ***************************************

void //DONE
MHsim::preRead(int _iIndex)
{
	int num = 0;

	int i = 0;

	while(_sComands[_iIndex][i] < '0' || _sComands[_iIndex][i] > '9')
	{
		i++;
	}

	while(_sComands[_iIndex][i] != '\0' && _sComands[_iIndex][i] != ' ')
	{
		num = num*10 + (_sComands[_iIndex][i] - 48);
		i++;
	}


	cout << ">>> read "<< num << " -> ";
	simRead(num);
}

void //DONE
MHsim::simRead(int _iNum)
{
	bool found = false;
	int line;
	int block;

	//search the adress _iNum
	for (int i = 0; i < _iCashSize; ++i)
	{
		if (_mCache[i].adress == _iNum)
		{
			found = true;
			cout << "HIT linha " << i/_iSizeBLock << endl;

			//if is LFU
			if(_viConfLines[5] == 3)
			{
				_viReplacement[i/_iSizeBLock]++;
			}
			//if is LRU
			else if(_viConfLines[5] == 4)
			{
				if(_viConfLines[3] == 2)
					preLRU(i);
				else if(_viConfLines[3] == 3)
				{
					int way;
					int first, last;

					way = _vMemory[_iNum].block % _iNWays;

					first = way*_iWaySize;
					last = way*_iWaySize + _iWaySize;
					preLRU(i, first, last);
				}
			}

			break;
		}
	}

	if(!found)
	{
		cout << "MISS -> ";

		//Direct Mapping
		if (_viConfLines[3] == 1)
		{
			line = _vMemory[_iNum].block%_iNLines;
			block = _vMemory[_iNum].block;

			mapping(line, _iNum);

			cout << "alocado na linha " << line;
			cout << " bloco " << block << " substituido\n";
		}

		//Fully Associative
		if (_viConfLines[3] == 2)
		{
			line = replacementPolicy(_iNum);
			block = _vMemory[_iNum].block;

			mapping(line, _iNum);

			cout << "alocado na linha " << line;
			cout << " bloco " << block << " substituido\n";
		}

		//Partially Associative
		if (_viConfLines[3] == 3)
		{
			line = partiallyAssociative(_iNum);
			block = _vMemory[_iNum].block;

			mapping(line, _iNum);

			cout << "alocado na linha " << line;
			cout << " bloco " << block << " substituido\n";
		}

	}
}

// ***************************************
// ********  MAPPING AND REPLACE  ********
// ***************************************

void //DONE
MHsim::mapping(int _iLine, int _iNum)
{
	//_iNum == line in the cache

	// line of cache * size of line/block 
	int indexCache = _iLine * _iSizeBLock;

	//num of the block * size of line/block
	int indexMemory = _vMemory[_iNum].block * _iSizeBLock;
			
	//verify if it is valid of not

	//*** VALID
	if(_mCache[indexCache].block != -1)
	{
		for (int i = 0; i < _iSizeBLock; ++i)
		{
			//save the updated values in the memory
			_vMemory[_mCache[indexCache + i].adress].value = _mCache[indexCache + i].value;
			
			//save the new information in the cache
			_mCache[indexCache + i].block = _vMemory[indexMemory + i].block;
			_mCache[indexCache + i].adress = _vMemory[indexMemory + i].adress;
			_mCache[indexCache + i].value = _vMemory[indexMemory + i].value;
		}
	}
	//*** INVALID
	else
	{
		for (int i = 0; i < _iSizeBLock; ++i)
		{
			//save the new information in the cache
			_mCache[indexCache + i].block = _vMemory[indexMemory + i].block;
			_mCache[indexCache + i].adress = _vMemory[indexMemory + i].adress;
			_mCache[indexCache + i].value = _vMemory[indexMemory + i].value;
		}
	}
}

// ***************************************
// **********  REPLACEMENT POLICY  *******
// ***************************************

int //DONE
MHsim::replacementPolicy(int _iNum)
{
	if(_viConfLines[5] == 1)
	{
		return rand() % _iNLines;
	}
	else if(_viConfLines[5] == 2)
	{
		return fifo();
	}
	else if(_viConfLines[5] == 3)
	{
		return LFU();		
	}
	else if(_viConfLines[5] == 4)
	{
		return LRU();		
	}

	return 0;
}

// ***************************************
// ********  PARCIALLY ASSOCIATIVE  ******
// ***************************************

int //DONE
MHsim::partiallyAssociative(int _iNum)
{
	int way;
	int first, last;

	way = _vMemory[_iNum].block % _iNWays;

	first = way*_iWaySize;
	last = way*_iWaySize + _iWaySize;

	return replacementPolicy(_iNum, first, last);
}

int //DONE
MHsim::replacementPolicy(int _iNum, int _iFirst, int _iLast)
{
	if(_viConfLines[5] == 1)
	{
		return rand() % _iNLines;
	}
	else if(_viConfLines[5] == 2)
	{
		return fifo(_iFirst, _iLast);
	}
	else if(_viConfLines[5] == 3)
	{
		return LFU(_iFirst, _iLast);		
	}
	else if(_viConfLines[5] == 4)
	{
		return LRU(_iFirst, _iLast);		
	}

	return 0;
}

int //DONE
MHsim::LFU(int _iFirst, int _iLast)
{
	int throne = _iFirst;

	cout << "\n >>> "<< throne << "  " << _viReplacement[throne] << "  -  ";

	for (int i = _iFirst+1; i < _iLast; ++i)
	{
		cout << i << "  " << _viReplacement[i] << endl;
		if(_viReplacement[i] < _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = 1;

	return throne;
}

void //DONE
MHsim::preLRU(int _iLine, int _iFirst, int _iLast)
{
	for (int i = _iFirst; i < _iLast; ++i)
	{
		if (_viReplacement[i] != 0)
		{
			_viReplacement[i]--;
		}
	}

	_viReplacement[_iLine] = _iWaySize;
}

int //DONE
MHsim::LRU(int _iFirst, int _iLast)
{
	int throne = _iFirst;

	if (_viReplacement[_iFirst] == 0)
	{
		_viReplacement[_iFirst] = _iWaySize;
		return 0;
	}

	for (int i = _iFirst + 1; i < _iLast; ++i)
	{
		if(_viReplacement[i] == 0)
		{
			_viReplacement[i] = _iWaySize;
			return i;
		}
		else if(_viReplacement[i] < _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = _iWaySize;
	return throne;
}

int //DONE
MHsim::fifo(int _iFirst, int _iLast)
{
	for (int i = _iFirst; i < _iLast; ++i)
	{
		if(_viReplacement[i] == 0)
		{
			_viReplacement[i]++;
			return i;
		}			
		_viReplacement[i]++;
	}

	int throne = _iFirst;

	for (int i = _iFirst + 1; i < _iLast; ++i)
	{
		if(_viReplacement[i] > _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = 1;

	return throne;
}


// ***************************************
// **********  LRU  ****FA****************
// ***************************************
void //DONE
MHsim::preLRU(int _iLine)
{
	for (int i = 0; i < _iNLines; ++i)
	{
		if (_viReplacement[i] != 0)
		{
			_viReplacement[i]--;
		}
	}

	_viReplacement[_iLine] = _iNLines;
}

int //DONE
MHsim::LRU()
{
	int throne = 0;

	if (_viReplacement[0] == 0)
	{
		_viReplacement[0] = _iNLines;
		return 0;
	}

	for (int i = 1; i < _iNLines; ++i)
	{
		if(_viReplacement[i] == 0)
		{
			_viReplacement[i] = _iNLines;
			return i;
		}
		else if(_viReplacement[i] < _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = _iNLines;
	return throne;
}

// ***************************************
// **********  LFU  ****FA****************
// ***************************************

int //DONE
MHsim::LFU()
{
	int throne = 0;

	for (int i = 1; i < _iNLines; ++i)
	{
		if(_viReplacement[i] < _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = 1;

	return throne;
}

// ***************************************
// **********  FIFO  ****FA***************
// ***************************************

int //DONE
MHsim::fifo()
{
	if(!_bFifoFull)
	{
		_bFifoFull = true;
		for (int i = 0; i < _iNLines; ++i)
		{
			if(_viReplacement[i] == 0)
			{
				_viReplacement[i]++;
				_bFifoFull = false;
				return i;
			}			
			_viReplacement[i]++;
		}
	}

	int throne = 0;
	_viReplacement[0]++;

	for (int i = 1; i < _iNLines; ++i)
	{
		_viReplacement[i]++;

		if(_viReplacement[i] > _viReplacement[throne])
		{
			throne = i;
		}
	}

	_viReplacement[throne] = 1;

	return throne;
}

// ***************************************
// **********  WRITE  ********************
// ***************************************

void //DONE
MHsim::preWrite(int _iIndex)
{
	int num1 = 0;
	int num2 = 0;

	int i = 0;

	while(_sComands[_iIndex][i] < '0' || _sComands[_iIndex][i] > '9')
	{
		i++;
	}

	while(_sComands[_iIndex][i] != ' ')
	{
		num1 = num1*10 + (_sComands[_iIndex][i] - 48);
		i++;
	}

	while(_sComands[_iIndex][i] < '0' || _sComands[_iIndex][i] > '9')
	{
		i++;
	}

	while(_sComands[_iIndex][i] != ' ' && _sComands[_iIndex][i] != '\0')
	{
		num2 = num2*10 + (_sComands[_iIndex][i] - 48);
		i++;
	}

	simWrite(num1, num2);
}

void //DONE
MHsim::simWrite(int _iAdress, int _iValue)
{
	cout << ">>> write " << _iAdress << " -> ";

	int line;
	bool hit = false;

	//seaching if the adress is in the cache
	for (int i = 0; i < _iCashSize; ++i)
	{
		if(_mCache[i].adress == _iAdress)
		{
			cout << "HIT linha " << i;
			hit = true;
			line = i;
			break;
		}
	}

	//if the adress is in the cashe
	if(hit)
	{
		_mCache[line].value = _iValue;
		//if is write through
		if(_viConfLines[6] == 2)
			_vMemory[_mCache[line].adress].value = _iValue;

		cout << " novo valor do endereco " << _iAdress << " = " << _iValue << endl;
	}
	//if the adress isn't in the cache
	else
	{
		simRead(_iAdress);
		simWrite(_iAdress, _iValue);
	}
}

// ***************************************
// **********  SHOW  *********************
// ***************************************

void //DONE
MHsim::simShow()
{
	//print what are in the cash
	cout << "\nShow ->\nCACHE\nLinha –Bloco-Endereço -Conteudo\n";

	for (int i = 0; i < _iCashSize; ++i)
	{
		cout << i/_iSizeBLock << "  - " << _mCache[i].block << " - ";
		cout << _mCache[i].adress << " - " << _mCache[i].value << endl;
	}


	//print what are in the memory
	cout << "\nMEMORIA PRINCIPAL\nBloco-Endereço -Conteudo\n";

	for (int i = 0; i < _iMemorySize; ++i)
	{
		cout << _vMemory[i].block << " - " << _vMemory[i].adress << " - ";
		cout << _vMemory[i].value << endl;
	}

	cout << endl;
}

bool //DONE
MHsim::comands()
{
	char comand;

	cout << "\nescreva o comando desejado:\n >>> w/W : Write\n >>> r/R : Read\n >>> s/S : Show\n >>> x/X : Exit\n";
	
	cin >> comand;

	if(comand == 'w' || comand == 'W')
	{
		int adress, value;
		cout << "escreva o endereco e o novo valor: \n";
		cin >> adress >> value;

		simWrite(adress, value);
		return true;
	}
	else if(comand == 'r' || comand == 'R')
	{
		int adress;
		cout << "Escreva o endereco para leitura: \n";
		cin >> adress;

		simRead(adress);
		return true;
	}
	else if(comand == 's' || comand == 'S')
	{
		simShow();
		return true;
	}
	else if(comand == 'x' || comand == 'X')
	{
		return false;
	}
	else
	{
		cout << "Escreva um comando valido!\n";
		return true;
	}
}