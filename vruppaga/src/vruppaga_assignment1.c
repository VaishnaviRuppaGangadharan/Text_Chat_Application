/**
 * @vruppaga_assignment1
 * @author  Vaishnavi Ruppa Gangadharan <vruppaga@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/includes.h"
#include "../include/globals.h"
#include "../include/functions.h"
#include "../include/clientProcess.h"
#include "../include/serverProcess.h"
#include "../include/global.h"
#include "../include/logger.h"

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	if(argc != 3) 
	{
		printf("please enter two argument c/s and PORT number");
		exit(-1);
	}
	if(*argv[1]=='s')
	{
		serverProcess(atoi(argv[2]));
	}
	else if(*argv[1]=='c')
	{
		clientProcess(atoi(argv[2]));
	}
	else
	{
		printf("Commands error, retry again!\n");
		exit(-1);
	}

	return 0;
}
