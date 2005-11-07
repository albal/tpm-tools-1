/*
 * The Initial Developer of the Original Code is International
 * Business Machines Corporation. Portions created by IBM
 * Corporation are Copyright (C) 2005 International Business
 * Machines Corporation. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Common Public License as published by
 * IBM Corporation; either version 1 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Common Public License for more details.
 *
 * You should have received a copy of the Common Public License
 * along with this program; if not, a copy can be viewed at
 * http://www.opensource.org/licenses/cpl1.0.php.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {

	char startup[] = {
        	0, 193,         /* TPM_TAG_RQU_COMMAND */
	        0, 0, 0, 12,    /* length */
	        0, 0, 0, 153,   /* TPM_ORD_Startup */
	        0, 1            /* ST_CLEAR */
	};
	int fd;
	int err;
	int rc = 1;

	fd = open( "/dev/tpm0", O_RDWR );
	if ( fd < 0 ) {
		printf( "Unable to open the device.\n" );
		goto out;	
	}

	err = write( fd, startup, sizeof(startup) );

	if ( err != sizeof( startup ) ){
		printf( "Error occured while writing the startup command: %d\n", errno );
		goto out;
	}
	
	err = read( fd, startup, sizeof(startup) );
	if ( err != 10 ) {
		printf( "Error occured while reading the startup result: %d %d %s\n", err, errno, strerror(errno));
		goto out;
	}

	/* ENDIANESS ISSUES HERE */
	//memcpy( &err, startup+6, 4 ); //for PPC64
	
	memcpy( &err+3, startup+6, 1);
	memcpy( &err+2, startup+7, 1);
	memcpy( &err+1, startup+8, 1);
	memcpy( &err, startup+9, 1);

	if ( err == 38 ) {
		printf( "TPM already started.\n" );
		goto out;
	}

	if ( err != 0 ) {
		printf( "TPM Error occured: %d\n", err );
		goto out;
	}

	rc = 0;
	printf( "Startup successful\n" );

out:
	close(fd);
        return rc;
}

