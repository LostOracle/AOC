#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


/*
 * Types and constants
 */
typedef struct VM
{
	int pc;
	int* mem;
	unsigned int mem_size;
	bool debug;
	bool halt;
} VM;

typedef struct Instruction
{
	int op;
	void (*func)( struct Instruction, VM*);
	int param_len;
	int param_modes[3];
	int param_addrs[3];
	int param_values[3];
} Instruction;

// Lookup table for parameter length;
static const int param_len[128] =
{
	0, 3, 3, 1, 1, 2, 2, 3,
	3, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};

/*
 * Debug/print
 */
void print_instruction( Instruction inst )
{
	printf( "opcode: %i, len: %i\n", inst.op, inst.param_len );
	for( int i = 0; i < inst.param_len; i++ )
	{
		printf( "  param %i mode:%i, addr:%i, value:%i\n", i, inst.param_modes[i], inst.param_addrs[i], inst.param_values[i] );
	}
}

void mem_dump( VM vm, int start, int end)
{
	for( int i = start; i < end; i++ )
	{
		printf( "a: %4i, d:%4i\n", i, vm.mem[i] );
	}
	printf("\n");
}

/*
 * Operations
 */

void halt( Instruction inst, VM* vm )
{
	vm->halt = true;
	vm->pc += 1 + inst.param_len;
}

void add( Instruction inst, VM* vm )
{
	vm->mem[inst.param_addrs[2]] = inst.param_values[0] + inst.param_values[1];
	vm->pc += 1 + inst.param_len;
}

void mult( Instruction inst, VM* vm )
{
	vm->mem[inst.param_addrs[2]] = inst.param_values[0] * inst.param_values[1];
	vm->pc += 1 + inst.param_len;
}

void input( Instruction inst, VM* vm )
{
	int tmp;
	scanf("%i", &tmp);
	vm->mem[inst.param_addrs[0]] = tmp;
	vm->pc += 1 + inst.param_len;
}

void output( Instruction inst, VM* vm )
{
	printf("%i\n", inst.param_values[0] );
	vm->pc += 1 + inst.param_len;
}

void jump_if_true( Instruction inst, VM* vm )
{
	if( inst.param_values[0] )
		vm->pc = inst.param_values[1];
	else
		vm->pc += 1 + inst.param_len;
}

void jump_if_false( Instruction inst, VM* vm )
{
	if( !inst.param_values[0] )
		vm->pc = inst.param_values[1];
	else
		vm->pc += 1 + inst.param_len;
}

void less_than( Instruction inst, VM* vm )
{
	if( inst.param_values[0] < inst.param_values[1] )
		vm->mem[inst.param_addrs[2]] = 1;
	else
		vm->mem[inst.param_addrs[2]] = 0;
	vm->pc += 1 + inst.param_len;
}

void equals( Instruction inst, VM* vm )
{
	if( inst.param_values[0] == inst.param_values[1] )
		vm->mem[inst.param_addrs[2]] = 1;
	else
		vm->mem[inst.param_addrs[2]] = 0;
	vm->pc += 1 + inst.param_len;
}

/*
 * Core
 */
void init_vm( VM* vm, unsigned int mem_size, bool debug )
{
	vm->pc = 0;
	vm->halt = false;
	vm->debug = debug;

	vm->mem_size = mem_size;
	vm->mem = malloc( mem_size );
	if( vm->mem == NULL )
	{
		printf("Failled to allocate memory for vm.\n");
	}
}

Instruction parse_instruction( VM vm )
{
	Instruction ret;

	int tmp = vm.mem[vm.pc];

	ret.op = ( tmp ) % 100 ;
	tmp/=100;

	switch( ret.op )
	{
		case 1:
			ret.func = &add;
			break;
		case 2:
			ret.func = &mult;
			break;
		case 3:
			ret.func = &input;
			break;
		case 4:
			ret.func = &output;
			break;
		case 5:
			ret.func = &jump_if_true;
			break;
		case 6:
			ret.func = &jump_if_false;
			break;
		case 7:
			ret.func = &less_than;
			break;
		case 8:
			ret.func = &equals;
			break;
		case 99:
			ret.func = &halt;
			break;
	}

	ret.param_len = param_len[ret.op];
	for( int i = 0; i < ret.param_len; i++ )
	{
		ret.param_modes[i] = tmp % 10;
		tmp/=10;

		switch( ret.param_modes[i] )
		{
			// Position mode
			case 0:
				ret.param_addrs[i] = vm.mem[vm.pc+1+i];
				ret.param_values[i] = vm.mem[ret.param_addrs[i]];
				break;
			// Immediate mode
			case 1:
				ret.param_addrs[i] = -1;
				ret.param_values[i] = vm.mem[vm.pc+1+i];
				break;
			default:
				printf( "bad mode: %i", ret.param_modes[i] );
				break;
		}
	}

	return ret;
}

void run_program( VM* vm )
{
	while( !vm->halt )
	{
		Instruction inst = parse_instruction( *vm );

		if( vm->debug )
		{
			printf( "Status: pc=%i, \n", vm->pc );
			print_instruction( inst );
		}

		inst.func( inst, vm );
	}

}

/*
 * Program input and main
 */

unsigned int read_program( char* filename, int** program )
{
	FILE *fptr;

	fptr = fopen( filename, "r" );
	if( fptr == NULL )
	{
		printf("fptr null\n");
		return -1;
	}

	fseek( fptr, 0, SEEK_END );
	int len = ftell( fptr );
	rewind( fptr );
	char* tmp = malloc( len * sizeof( char ) );
	if( tmp == NULL )
	{
		printf("Failled to allocate memory for temp buffer.\n");
		return 0;
	}

	char ch;
	int i = 0;
	int mem_size = 1;
	while( !feof( fptr ) )
	{
		ch = fgetc(fptr);
		if( ch == ',' )
			mem_size++;
		tmp[i++] = ch;
	}
	mem_size *= sizeof(int);

	*program = malloc( mem_size );
	if( *program == NULL )
	{
		printf("Failled to allocate memory for program buffer.\n");
		return 0;
	}

	char* token = strtok( tmp, "," );
	i = 0;
	while( token )
	{
		(*program)[i++] = atoi( token );
		token = strtok( NULL, "," );
	}

	free(tmp);

	return mem_size;
}

int main( int argc, char** argv )
{
	int* program = NULL;
	unsigned int mem_size = read_program( argv[1], &program );

	VM vm;
	init_vm( &vm, mem_size, false );

	// copy the program into the working memory
	memcpy( vm.mem, program, mem_size );

	run_program( &vm );

	free( program );
	free( vm.mem );
}

