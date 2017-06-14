#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

using namespace std;


class MHsim{

	private:

		struct stLine{
			int block;
			int adress;
			int value;
		};

		int _iSizeBLock; //how mane word is in one block
		int _iNLines; //how many line is in the cache
		int _iNBlock; //how many block is in the memory
		
		int _iCashSize; //the size of the cash
		int _iMemorySize; //the size of the memory

		int _viConfLines[7]; //confing.txt in numbers

		//*** Replacement Policy
		int * _viReplacement;
		bool _bFifoFull = false;

		//*** Partially Associative
		int _iNWays;
		int _iWaySize;


		stLine * _mCache; //what are in the Cache
		stLine * _vMemory; //what are in the memory

		int _iNComands;
		string * _sComands;

		//*****************************
		//******** Fuctions ***********
		//*****************************


		//transform the strings of _vsConfLines to numbers
		//and store in _viConfLines
		void transNum(int i, string line);

		//*****  DEBUG FUCTIONS  ******
		//print the lines of "confing"
		void printConfig();
		//print _sComands
		void printComands();

		//**** READ FUCTIONS ****
		//get the number of the adress
		void preRead(int _iIndex);
		//do the fuction of Read
		void simRead(int _iNum);

		//**** MAPPING **********
		void mapping(int _iLine, int _iNum);

		// ********  PARCIALLY ASSOCIATIVE  ******
		int partiallyAssociative(int _iNum);
		//Replacement policy
		int replacementPolicy(int _iNum, int _iFirst, int _iLast);
		int fifo(int _iFirst, int _iLast);
		int LFU(int _iFirst, int _iLast);
		//update who was the line of recently used
		void preLRU(int _iLine, int _iFirst, int _iLast);
		int LRU(int _iFirst, int _iLast);

		// ********  FULLY ASSOCIATIVE  ******
		//Replacement policy
		int replacementPolicy(int _iNum);
		int fifo();
		int LFU();
		//update who was the line of recently used
		void preLRU(int _iLine);
		int LRU();

		//**** WRITE FUCTIONS ***
		//get the number of the adress and the value
		void preWrite(int _iIndex);
		//do the fuction of Write
		void simWrite(int _iAdress, int _iValue);

		//do the fuction of Show
		void simShow();



	public:
		//constructor without parameters
		MHsim();

		//destructor
		~MHsim();

		//call all the comands in the "input.txt"
		void simulate();

		bool comands();




};

#endif