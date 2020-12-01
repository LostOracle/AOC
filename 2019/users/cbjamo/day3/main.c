#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct XY
{
	int x;
	int y;
} XY;

typedef struct Path
{
	XY cursor;
	XY points[100];
	int length;
} Path;

char** read_input( char* filename )
{
	FILE *fptr;

	char ch;

	fptr = fopen( filename, "r" );
	if( fptr == NULL )
	{
		printf("fptr null\n");
		return NULL;
	}

	fseek( fptr, 0, SEEK_END );
	int len = ftell( fptr );
	rewind( fptr );
	char* tmp = malloc( len * sizeof( char ) );

	int i = 0;
	while( !feof( fptr ) )
	{
		ch = fgetc(fptr);
		tmp[i++] = ch;
	}

	char** inputs = malloc( 10 * sizeof(char*) );
	i = 0;
	char* token = strtok( tmp, "\n" );
	while( token )
	{
		inputs[i] = malloc( strlen( token ) );
		strcpy( inputs[i++], token );
		token = strtok( NULL, "\n" );
	}

	free( tmp );

	return inputs;
}

Path parse_path( char* input )
{
	Path ret;
	ret.cursor.x = 0;
	ret.cursor.y = 0;
	ret.length = 0;

	char* token = strtok( input, "," );
	while( token )
	{
		char dir = 'x';
		int dist = 0;
		XY point;
		sscanf( token, "%c%i,", &dir, &dist );

		switch( dir )
		{
			case 'U':
				point.x = dist;
				point.y = 0;
				break;
			case 'D':
				point.x = -dist;
				point.y = 0;
				break;
			case 'R':
				point.x = 0;
				point.y = dist;
				break;
			case 'L':
				point.x = 0;
				point.y = -dist;
				break;
			default:
				printf("Bad dir\n");
		}
		ret.points[ret.length++] = point;	

		token = strtok( NULL, "," );
	}
	return ret;
}

void print_path( Path path )
{
	printf( "Len: %i\n", path.length );
	for(int i = 0; i < path.length; i++ )
	{
		printf("l: %i, x: %i, y: %i \n", i, path.points[i].x, path.points[i].y );
	}
}

int main( int argc, char** argv )
{
	char** inputs = read_input( argv[1] );

	Path path[10];
	for( int i = 0; i < 2; i++ )
	{
		printf("%i %s\n", i, inputs[i] );
		path = parse_path( inputs[i] );
		print_path( path );
	}

}

