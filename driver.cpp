#include <iostream>

#include "simulator.h"

using namespace std;

int main()
{
	MHsim _MHsim;
	bool loop = true;

	_MHsim.simulate();

	while(loop)
	{
		loop = _MHsim.comands();
	}

	return 0;
}