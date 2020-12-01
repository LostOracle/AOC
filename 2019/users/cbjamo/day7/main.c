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
	int64_t* mem;
	size_t mem_size;
	bool debug;
	bool halt;
	bool pause;
	size_t in_pos;
	int64_t in[10];
	int64_t out;
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
	vm->mem[inst.param_addrs[0]] = vm->in[vm->in_pos];

	printf("p: %li i: %li\n", vm->in_pos, vm->in[vm->in_pos] );
	vm->in_pos++;

	vm->pc += 1 + inst.param_len;
}

void output( Instruction inst, VM* vm )
{
	vm->out = inst.param_values[0];

	vm->pc += 1 + inst.param_len;

	vm->pause = true;
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

void hcf( Instruction inst, VM* vm )
{
	halt( inst, vm );
	while( true );
}

/*
 * Core
 */
size_t init_vm( VM* vm, int32_t id, size_t mem_size, bool debug )
{
	vm->id = id;

	vm->pc = 0;
	vm->pause = false;
	vm->halt = false;
	vm->debug = debug;

	vm->in_pos = 0;

	vm->mem_size = mem_size;
	vm->mem = malloc( mem_size );
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
	int mem_size = 1;
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
		(*program)[i++] = atoi( token );
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

	int i, j, k, l, m, n = 0;

	int64_t phase[120][5];
	for( i = 5; i < 10; i++)
	for( j = 5; j < 10; j++)
	for( k = 5; k < 10; k++)
	for( l = 5; l < 10; l++)
	for( m = 5; m < 10; m++)
	{
		if (i == m || i == l || i == k || i == j  || j == m || j == l || j == k || k == m || k == l || l == m)
			continue;

		phase[n][0] = i;
		phase[n][1] = j;
		phase[n][2] = k;
		phase[n][3] = l;
		phase[n][4] = m;
		n++;
	}

	int64_t max = 0;
	VM vms[5];

	for( n = 0; n < 120; n++ )
	{

		for( uint32_t r = 0; r < 5; r++ )
		{
			if( !init_vm( &vms[r], r, mem_size, true ) )
				return -3;	

			// copy the program irto the workirg memory
			memcpy( vms[r].mem, program, mem_size );
		}

		printf("%i\n", n);

		bool halt = false;
		int first = 0;
		while( !halt )	
		{

			for( uint32_t v = 0; v < 5; v++ )
			{
				vms[v].in_pos = 0;
				if( first < 5)
				{
					vms[v].in[0] = phase[n][v];

					if( v == 0 )
						vms[0].in[1] = 0;
					else
						vms[v].in[1] = vms[v-1].out;
					first++;
				}
				else
				{
					if( v == 0 )
						vms[0].in[0] = vms[4].out;
					else
						vms[v].in[0] = vms[v-1].out;
				}

				halt = run_program( &vms[v], false );
				vms[v].pause = false;

					//getchar();
			}
		}

		if( max < vms[4].out )
			max = vms[4].out;	

	}
	printf("max:%li\n", max);

	free( program );

	for( uint32_t i = 0; i < 5; i++ )
		free( vms[i].mem );
}


