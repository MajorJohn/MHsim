MHsim - Memory Hierarchy

UFRN - Federal University of Rio Grande do Norte

Made by João Vitor Filgueira Albuquerque

This project is to simulate how cache memory works in a
diferents ways with the main memory.

How to test:
	1. configure the cache and the main mamory
	   in the "config.txt"

			0 - size of the block
			1 - number of lines (in the cache)
			2 - how many blocks the main memory has
			3 - mapping way (1 -direct; 2 - fully 
				associative; 3 - partially associative)
			4 - Number of way (to partially associative)
			5 - Replacement policy (1 - random; 2 - fifo;
				 3 - LFU; 4 - LRU)
			6 - Write policy (1 – Write-back;
				2 – Write-Through)

	2. configure the comands you want in the "input.txt"
			Command> Read 0 (try to read in the cache
							 the adress 0)
			Command> write 5 10 (write the value 10 in
								 the adress 5)
			Command> Show (show how are your cahce and
						   main memory)
	3. open the terminal and go to the file
	4. write "make", than "./sim"


ps.:

example of "config.txt"

	4
	4
	16
	3
	2
	2
	1

example of "input.txt"

	Command> Read 0
	Command> Read 28
	Command> Read 23
	Command> Read 25
	Command> Read 10
	Command> Read 26
	Command> Read 16
	Command> Read 30
	Command> write 7 10
	Command> Show
