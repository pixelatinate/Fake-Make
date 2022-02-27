// Lab 3: huff_dec.c
// This lab is a restricted and pared-down version of "make".
//      See http://web.eecs.utk.edu/~jplank/plank/classes/cs360/360/labs/Lab-4-Fakemake/index.html
//      for more information and lab specifications. 

// COSC 360
// Swasti Mishra 
// Feb 28, 2022
// James Plank 

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <sys/stat.h>

# include "dllist.h"
# include "fields.h"

// Finds the length of a list- faster than just traversing and counting every time lol
int findLength( Dllist list ){
	Dllist tmp ;
	int length = 0 ;
	dll_traverse( tmp, list ) length = ( 1 + length + strlen( tmp->val.s ) ) ;
	return length ;
}

// Error checks the C files and remakes the string
int processCFiles( Dllist CList, Dllist FList, int HCheck ){
	Dllist tmp ;
	struct stat buff ;

	int maxTime ;
	int remadeFiles ;
	int FLength = findLength(FList) ;

	// Go through the list 
	dll_traverse( tmp, CList ){
		// Check if the path is right 
		if( stat( tmp->val.s, &buff ) < 0 ){
			fprintf( stderr, "fmakefile: %s: No such file or directory\n", tmp->val.s ) ;
			return -1 ;
		}
		
		int buffTime = buff.st_mtime ;
		
		// Fix the file name 
		char *CFile = strdup(tmp->val.s) ;
		char *OFile = strdup(CFile) ;
		OFile[ strlen(CFile) - 1 ] = 'o' ;
		
		// Find the max time and recompile
		if( stat(OFile, &buff) < 0 || buff.st_mtime < buffTime || buff.st_mtime < HCheck ){
			char *compiledObject = (char *) malloc( ( strlen(OFile) + FLength + 7 ) * sizeof(char) ) ;
			strcpy( compiledObject, "gcc -c" ) ;
			int CLength = strlen(compiledObject) ;
			Dllist tmp2 ;

			dll_traverse( tmp2, FList ){
				strcat( compiledObject + CLength, " " ) ;
				strcat( compiledObject + CLength, tmp2->val.s ) ;
				CLength = strlen(compiledObject) ;
			}

			strcat( compiledObject + CLength, " " ) ;
			strcat( compiledObject + CLength, CFile ) ;
			
			// Print our compilation string and run it 
			printf( "%s\n", compiledObject ) ;
			if( system(compiledObject) ){
				fprintf( stderr, "Command failed.  Exiting\n" ) ;
				return -1 ;
			}
			remadeFiles = 1 ;
		}
		else{
			if( buff.st_mtime > maxTime ){
				maxTime = buff.st_mtime ;
			}
		}
	}

	// Tells us if we had to remake the files or not
	if( remadeFiles == 1 ){
		return 1 ;
	}
	if( remadeFiles != 1 ){
		return maxTime ;
	}
}

int main( int argc, char* argv[] ) {
	
	// The input structure
	IS is ;

	// Lines of the description file
	Dllist CList = new_dllist() ;
	Dllist HList = new_dllist() ;
	char  *EName = NULL ;
	Dllist FList = new_dllist() ;
    Dllist LList = new_dllist() ;

	// The default file descriptor versus a specific file descriptor
	if( argc == 1 ){
		is = new_inputstruct( "fmakefile" ) ;
	}
	else{
		is = new_inputstruct( argv[1] ) ;
	}

	// Reads the input into the lists
	while( get_line(is) >= 0 ){
		if( is->NF == 0 ){
			continue ;
		}
		for( int i = 1 ; i < is->NF ; i++ ){
			if( strcmp( is->fields[0], "C" ) == 0 ){
				dll_append( CList, new_jval_s( strdup( is->fields[i] ) ) ) ;
			}
			if( strcmp( is->fields[0], "H" ) == 0 ){
				dll_append( HList, new_jval_s( strdup( is->fields[i] ) ) ) ;
			}
			if( strcmp(is->fields[0], "E") == 0){
				if( EName != NULL ){
					fprintf( stderr, "fmakefile (%d) cannot have more than one E line\n", is->line ) ;
					return 1 ;
				}
				else{
					EName = strdup( is->fields[i] ) ;
				}
			}
			if( strcmp( is->fields[0], "L" ) == 0 ){
				dll_append( LList, new_jval_s( strdup( is->fields[i] ) ) ) ;
			}
			if( strcmp( is->fields[0], "F" ) == 0 ){
				dll_append( FList, new_jval_s( strdup( is->fields[i] ) ) ) ;
			}
		}
	}

	// Error in case EName is still NULL
	if( EName == NULL ){
		fprintf( stderr, "No executable specified\n" ) ;
		return 1 ;
	}

	// Error checks the HList
	Dllist tmpH ;
	struct stat buffH ;

	int inMakeH ;
	int maxTimeH ;

	if( dll_empty(HList) == 0 ){
		inMakeH = stat( HList->flink->val.s, &buffH ) ;
		
		// Check if the path is right
		if( inMakeH < 0 ){
			fprintf( stderr, "fmakefile: %s: No such file or directory\n", HList->flink->val.s ) ;
			return -1 ;
		}
		maxTimeH = buffH.st_mtime ;
	}

	// Calculates the maximum time of the header files
	dll_traverse( tmpH, HList ){
		inMakeH = stat( tmpH->val.s, &buffH ) ;
		if( inMakeH < 0 ){
			fprintf( stderr, "fmakefile: %s: No such file or directory\n", tmpH->val.s ) ;
			return -1 ;
		}
		if( maxTimeH < buffH.st_mtime ){
			maxTimeH = buffH.st_mtime ;
		}
	}
	
	int HCheck = maxTimeH ;
	if( HCheck == 1 ){
		return 1 ;
	}

	// Error checks the CList
	int CCheck = processCFiles( CList, FList, HCheck ) ;
	if ( CCheck == -1 ){
		return -1 ;
	}

	// You have to make this structure here- it just checks if 
	//		the line is in the string 
	struct stat buff ;
	int inMake = stat( EName, &buff ) ;

	// Time to compile the file!!
	if( CCheck == 1 || CCheck > buff.st_mtime || inMake < 0 ){
		Dllist tmp ;

		// Add up lists
		int totalLength = findLength(CList) + 
						  findLength(FList) + 
						  strlen( EName )   +
						  findLength(LList) ;

		// The 7 is for "gcc -o" and its NULL character
		char *compiledObject = malloc( ( totalLength + 7 ) * sizeof( char ) ) ;
		int cObjectLength    = strlen( compiledObject ) ;
		
		// Copy spaces into the compile string 
		strcpy( compiledObject, "gcc -o" ) ;
		strcat( compiledObject + cObjectLength, " " ) ;
		strcat( compiledObject + cObjectLength, EName ) ;
		cObjectLength = strlen( compiledObject ) ;
		
		// Copy F lines into the compile string
		dll_traverse( tmp, FList ){
			strcat( compiledObject + cObjectLength, " " ) ;
			strcat( compiledObject + cObjectLength, tmp->val.s ) ;
			cObjectLength = strlen( compiledObject ) ;
		}
		
		// Copy C lines into the compile string
		dll_traverse( tmp, CList ){
			strcat( compiledObject + cObjectLength, " " ) ;
			char *CName = strdup( tmp->val.s ) ;
			CName[ strlen( CName ) - 1 ] = 'o' ;
			strcat( compiledObject + cObjectLength, CName ) ;
			cObjectLength = strlen( compiledObject ) ;
		}
		
		// Copy L lines into the compile string
		dll_traverse( tmp, LList ){
			strcat( compiledObject + cObjectLength, " " ) ;
			strcat( compiledObject + cObjectLength, tmp->val.s ) ;
			cObjectLength = strlen( compiledObject ) ;
		}

		// Print our string!!
		printf( "%s\n", compiledObject ) ;
		
		// If it didn't work, give up :(
		if ( system( compiledObject) != 0 ){
			fprintf(stderr, "Command failed.  Fakemake exiting\n");
			return 1 ;
		}
	}
	else{
		printf( "%s up to date\n", EName ) ;
	}

	return 0 ;
}
