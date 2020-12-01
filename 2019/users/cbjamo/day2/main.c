#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool read_program( char* filename, int* program )
{
	FILE *fptr;

	char ch;
	char tmp[1000];

	fptr = fopen( filename, "r" );
	if( fptr == NULL )
	{
		printf("fptr null\n");
	}
	ch = fgetc(fptr);
	int i = 0;
	while( ch != EOF )
	{
		tmp[i++] = ch;
		ch = fgetc(fptr);
	}

	char* token = strtok( tmp, "," );
	i = 0;
	while( token )
	{
		program[i++] = atoi( token );
		token = strtok( NULL, "," );
	}

	return true;
}

int run_program( int* program )
{
	int pc = 0;
	for( pc = 0; pc < 65536; pc=pc+4 )
	{
		int op = program[pc];
		int a1 = program[pc+1];
		int a2 = program[pc+2];
		int dst = program[pc+3];
		bool halt = false;
		
		switch( op )
		{
			case 1:
				program[dst] = program[a1] + program[a2];
				break;
			case 2:
				program[dst] = program[a1] * program[a2];
				break;
			case 99:
				halt = true;
				break;
		}

		if( halt )
			break;
	}
	return program[0];
}

int main( int argc, char** argv )
{
	//TODO: make the program memory dynamic
	int program[65536];
	int programbac[65536];

	read_program( argv[1], programbac );
	// copy the program into the working memory
	memcpy( program, programbac, 65536 );

	// Part 1
	program[1] = 12;
	program[2] = 2;
	run_program( program );
	printf( "Part 1: %i\n", program[0] );
	// Reset the working memory for part 2
	memcpy( program, programbac, 65536 );

	// Part 2
	bool pass = false;
	int noun;
	int verb;
	for( noun = 0; noun <= 99; noun++ )
	{
		for( verb = 0; verb <= 99; verb++ )
		{
			memcpy( program, programbac, 65536 );
			program[1] = noun;
			program[2] = verb;
			run_program( program );

			if( program[0] == 19690720 )
			{
				pass = true;
				break;
			}
		}
		if( pass )
			break;
	}
	printf( "Part 2 noun:%i, verb:%i\n", noun, verb );
}

