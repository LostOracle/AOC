#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>

/*
 * Types and constants
 */

typedef struct VM
{
	int32_t id;
	size_t pc;
	size_t rel_base;
	int64_t* mem;
	size_t mem_size;
	bool debug;
	bool halt;
	bool pause;
	FILE* in;
	FILE* out;
} VM;

typedef struct Instruction
{
	uint32_t op;
	void (*func)( struct Instruction, VM*);
	int32_t param_len;
	int32_t param_modes[3];
	size_t param_addrs[3];
	int64_t param_values[3];
} Instruction;

// Lookup table for parameter length;
static const uint32_t param_len[128] =
{
	0, 3, 3, 1, 1, 2, 2, 3,
	3, 1, 0, 0, 0, 0, 0, 0,
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
		printf( "  param %i mode:%i, addr:%li, value:%li\n", i, inst.param_modes[i], inst.param_addrs[i], inst.param_values[i] );
	}
}

void mem_dump( VM vm, size_t start, size_t end)
{
	for( size_t i = start; i < end; i++ )
	{
		printf( "a: %4li, d:%4li\n", i, vm.mem[i] );
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
	int64_t tmp;
	fscanf( vm->in, "%li", &tmp );
	vm->mem[inst.param_addrs[0]] = tmp;

	vm->pc += 1 + inst.param_len;
}

void output( Instruction inst, VM* vm )
{
	fprintf( vm->out,"%li\n", inst.param_values[0] );

	vm->pause = true;
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

void relative_base( Instruction inst, VM* vm )
{
	vm->rel_base += inst.param_values[0];

	vm->pc += 1 + inst.param_len;
}

void hcf( Instruction inst, VM* vm )
{
	halt( inst, vm );
	while( true );
}

/*
 * Core
 */
size_t init_vm( VM* vm, int32_t id, FILE* in, FILE* out, size_t mem_size, bool debug )
{
	vm->id = id;

	vm->pc = 0;
	vm->rel_base = 0;

	vm->pause = false;
	vm->halt = false;
	vm->debug = debug;

	vm->in = in;
	vm->out = out;

	vm->mem_size = mem_size;
	vm->mem = malloc( mem_size*2 );
	memset( vm->mem, 0, mem_size*2 );
	if( vm->mem == NULL )
	{
		printf("Failled to allocate memory for vm.\n");
		return 0;
	}

	return vm->mem_size;
}

Instruction parse_instruction( VM vm )
{
	Instruction ret;

	int64_t tmp = vm.mem[vm.pc];

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
		case 9:
			ret.func = &relative_base;
			break;
		case 99:
			ret.func = &halt;
			break;
		default:
			printf("Invalid Opcode, HCF\n");
			ret.func = &hcf;
			break;
	}

	ret.param_len = param_len[ret.op];
	for( int32_t i = 0; i < ret.param_len; i++ )
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
			// Relative base mode
			case 2:
				ret.param_addrs[i] = vm.rel_base + vm.mem[vm.pc+1+i];;
				ret.param_values[i] = vm.mem[ret.param_addrs[i]];
				break;
			default:
				printf( "bad mode: %i", ret.param_modes[i] );
				break;
		}
	}

	return ret;
}

bool run_program( VM* vm, bool single_step )
{
	while( !(vm->halt | vm->pause) )
	{
		Instruction inst = parse_instruction( *vm );

		if( vm->debug )
		{
			printf( "Status: id=%i, pc=%li, \n", vm->id, vm->pc );
			print_instruction( inst );
		}

		inst.func( inst, vm );

		if( single_step )
			getchar();
	}

	return vm->halt;
}

/*
 * Program input and main
 */

unsigned int read_program( char* filename, int64_t** program )
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
	size_t mem_size = 1;
	while( !feof( fptr ) )
	{
		ch = fgetc(fptr);
		if( ch == ',' )
			mem_size++;
		tmp[i++] = ch;
	}
	mem_size *= sizeof(int64_t);

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
		(*program)[i++] = atol( token );
		token = strtok( NULL, "," );
	}

	free(tmp);

	return mem_size;
}

void print_filename( FILE* f )
{
    char fname[FILENAME_MAX], link[FILENAME_MAX] = {0};
    sprintf(fname, "/proc/self/fd/%d", fileno(f));
    if(readlink(fname, link, sizeof link - 1) > 0)
        printf("File name: %s\n", link);
}

int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		printf( "usage: %s <intcode program>\n", argv[0] );
		return -1;
	}

	int64_t* program = NULL;
	size_t mem_size = read_program( argv[1], &program );
	if( mem_size == 0 )
		return -2;

	VM vm;
	init_vm( &vm, 0, stdin, stdout, mem_size, false );
	memcpy( vm.mem, program, mem_size );
	
	run_program( &vm, false );

	free( program );

	free( vm.mem );
}


