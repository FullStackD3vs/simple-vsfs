#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "functions/vsfs.h"


char *pathDelimiter = "/";
char *currDir;						// Current Directory
void *disk;
uint32_t currInode;

uint32_t vsfs_create (char *name, uint8_t ft) {

    Inode* cur = getInode(currInode);
    for(uint32_t i = 0; i < DIRECTCNT; i++){
        if(cur->ptr[i] != -1){
			DirEntry* dataBlock;
            for(dataBlock= getDataBlock(cur->ptr[i]); dataBlock != NULL; dataBlock =
                    dataBlock->next){
                if(strcmp(dataBlock->fileName,name) == 0){
                    printf("File with this name exists");
                    return -2;
                }
            }
        }
    }

    uint32_t newInode;
    if(strcmp(currDir,"") == 0){
        newInode = currInode;
    }
    else{
        newInode = getFreeInodeNb();
    }
    if(newInode < 0){
        return -1;
    }
    Inode* newptr = getInode(newInode);
    for(uint32_t i = 0; i < DIRECTCNT; i++){
        newptr->ptr[i] = -1;
    }

    if(ft == FT_DIR){
        newptr->type = FT_DIR;
    }

    else if(ft == FT_FIL){
        newptr->type = FT_FIL;
    }

    DirEntry* dataBlock;

    for(uint32_t i = 0; i < DIRECTCNT; i++){
        if(cur->ptr[i] != -1){
            for(dataBlock = getDataBlock(cur->ptr[i]); dataBlock->next != NULL; dataBlock = dataBlock->next){
                if(dataBlock->iNodeNb == -1){
                    dataBlock->iNodeNb = newInode;
                    strcpy(dataBlock->fileName,name);
                    return 0;
                }
            }

            if(dataBlock->iNodeNb == -1){
                dataBlock->iNodeNb = newInode;
                strcpy(dataBlock->fileName,name);
                return 0;
            }

            DirEntry* tmp = dataBlock;

            if(dataBlock != getDataBlock(cur->ptr[i])){
                dataBlock += sizeof(DirEntry) + 4;
            }

            if((void*)getDataBlock(cur->ptr[i]) + BLOCKSIZE - (void*)dataBlock > sizeof(DirEntry) + 4){
                tmp->next = dataBlock;
                dataBlock->iNodeNb = newInode;
                strcpy(dataBlock->fileName, name);
                dataBlock->next = NULL;
                return 0;
            }
        }

        else if(cur->ptr[i] == -1){
            uint32_t newBlock = getFreeDataBlockNb();
            cur->ptr[i] = newBlock;
            dataBlock = getDataBlock(newBlock);
            strcpy(dataBlock->fileName,name);
            dataBlock->iNodeNb = newInode;
            dataBlock->next = NULL;
            return 0;
        }
    }
    return -1;

}//create

/************************************************************************
 * Mount disk and its file system: we don't attach the new file system  *
 * to a mount point (as Unix does). We only prepare for vsfs to be used *
 * - verify the signature in superblock                                 *
 * - create the root directory (named "/")                              *
 * - initialize currDir to the root                                     *
 ***********************************************************************/
void vsfs_mount () {
	SuperBlock* sb = (SuperBlock*) disk;
	assert(sb->signature == MAGICNB);
	currInode = getFreeInode();
	assert(vsfs_create("/",FT_DIR) == 0);
	strcat(currDir,pathDelimiter);
}
uint32_t getFreeInode(){
    //create the node
    Inode *newNode  = (Inode*) disk;
    //systematically give this a number;
	newNode->number = 1;
	newNode->type = 0; //create a default as directory
	uint8_t i=0;
	for(i =0; i<DIRECTCNT; i++){
		newNode->ptr[i] = -1;
	}

}
Inode* getInode(uint32_t nodeNumber){
	//iterate and fine the node
	for()
}   
//vsfs_mount


/***********************************************
 * Create a virtual disk with blockCnt blocks. *
 **********************************************/
void vsfs_initDisk (uint32_t blockCnt) {
	uint32_t diskSize = parameters.blockSize * blockCnt;
	disk = malloc (diskSize);
	assert (disk != NULL);

	// zero the disk 
	memset (disk, 0, diskSize);

	// Create superblock with ad hoc values
	SuperBlock *sb = (SuperBlock*) disk;
	sb->signature = MAGICNB;
    sb->blockCnt = blockCnt;
    sb->blockSize = parameters.blockSize;
	sb->iNodeBMSize = parameters.iNodeBMSize;
	sb->dataBMSize = parameters.dataBMSize;	
    sb->iNodeTabSize = parameters.iNodeTabSize;
	
 	sb->iNodeSize = sizeof(Inode);

	// Reserve space for the currDir. Will be set up in vsfs_mount
	currDir = (char *)(malloc (FILENAME_LENGTH*sizeof(char)));
	assert (currDir != NULL);

}//vsfs_initDisk

/******************************************
 * Create a file in the current directory *
 * Parameters: name of file               *
 *             file type                  * 
 * return -1 if no space available        *
 *        -2 duplicate file name          *
 *****************************************/

// getInode(dataBlock->iNodeNb)->type ==
                // ft

int getCmd (char *cmd) {
	for (int i=0; i<CMDCNT; i++) {
		Symbol *s = &lookupTable [i];
		if (strcmp (s->cmd, cmd) == 0)
			return s->val;
	}
	return BADCMDE;
}//getCmd

void help (){
	printf ("help\t\tdisplay this file \n");
	printf ("ls\t\tlist files in current directory\n");
	printf ("cd\t\tchange directory\n");
	printf ("make xxx\tcreate file xxx in current directory\n");
	printf ("mkdir xxx\tcreate directory xxx in current directory\n");
	printf ("rmf xxx\t\tremove file xxx from current directory\n");
	printf ("rmd xxx\t\tremove directory xxx from current directory\n");
	printf ("q\t\tQuit simulation\n");
	printf ("\n");
	printf("clear \t clear the screen");
} // help

/******************************************************
 * Command line has 2 elements:                       *
 * a command followed by a parameter to the command.  *
 * get them and call appropriate routine.             *
 *****************************************************/
int parseAndExecute (char *cmdLine) {
	int quit = 0;
	uint32_t retVal;
	char *cmde = malloc (CMDE_LENGTH * sizeof (char));
	assert (cmde != 0);
	char *param1 = malloc (CMDE_LENGTH * sizeof (char));
	assert (param1 != 0);
	char *param2 = malloc (CMDE_LENGTH * sizeof (char));
	assert (param2 != 0);
	char *dupCmdLine = malloc (CMDE_LENGTH * sizeof (char));
	assert (dupCmdLine != 0);

	strcpy (dupCmdLine, cmdLine);

	// Get command
  	char *token = strsep (&dupCmdLine, " ");
	if (token == NULL) {
		printf ("command %s not found\n", cmdLine);
		return 0;
	}
	strcpy (cmde, token);
	// Get parameter 1 (may be null)
   	token = strsep (&dupCmdLine, " ");
	if (token == NULL) {
		param1[0] = '\0';
		param2[0] = '\0';
	} else {
		strcpy (param1, token);
		// Get parameter 2 (must be null)
   		token = strsep (&dupCmdLine, " ");
		if (token != NULL) {
			printf ("%s: bad operand\n", token);
			return 0;
		}
		param2[0] = '\0';
	}

	switch (getCmd (cmde)) {
		case QUIT:
				printf ("Simulation terminated.\n");
				quit = 1;
				break; 
		case HELP:
				if (param1[0] != '\0') {
					printf ("%s: bad operand\n", param1);
				} else {
					help ();
				}
				break; 
		case LS:
				if (param1[0] != '\0') {
					printf ("%s: bad operand\n", param1);
				} else {
					// vsfs_LS ();
				}
				break; 
		case CD:
				if (param1[0] == '\0') {
					printf ("%s: missing operand\n", cmdLine);
				} else {
					// vsfs_CD (param1);
				}
				break;
		case MAK:
				if (param1[0] == '\0') {
					printf ("%s: missing operand\n", cmdLine);
				} else {
					// retVal = vsfs_create (param1, FT_FIL);
					// if (retVal != 0)
					// 	printf ("Error %d in creation of %s\n", retVal, param1);
				}
				break;
		case MKD:
				if (param1[0] == '\0') {
					printf ("%s: missing operand\n", cmdLine);
				} else {
					// retVal = vsfs_create (param1, FT_DIR);
					// if (retVal != 0)
					// 	printf ("Error %d in creation of %s\n", retVal, param1);
				}
				break;
		case RMF:
				if (param1[0] == '\0') {
					printf ("%s: missing operand\n", cmdLine);
				} else {
					// retVal = vsfs_RMF (param1);
					// if (retVal != 0)
					// 	printf ("Error %d in removing %s\n", retVal, param1);
				}
				break;
		case RMD:
				if (param1[0] == '\0') {
					printf ("%s: missing operand\n", cmdLine);
				} else {
					// retVal = vsfs_RMD (param1);
					// if (retVal != 0)
					// 	printf ("Error %d in removing %s\n", retVal, param1);
				}
				break;
		case CLEAR:
				system("clear");
				break;
		case BADCMDE:
				printf ("command %s not found\n", cmdLine);
	}
	free (cmde);
	free (param1);
	free (param2);
	free (dupCmdLine);
	return quit;

}//parseAndExecute

int main (int argc, char *argv[]) {
	uint32_t retVal;
	char *cmdLine = NULL;			// Command line
	size_t len = 0;					// Number of char read

	// Read parameters
	initParams ();
	if (argc == 2) {
		getParams (argv[1]);
	}

	currDir = (char *)(malloc (128*sizeof(char)));
	assert (currDir != NULL);
	vsfs_initDisk (100);
	vsfs_mount ();
	printf ("%s: ", currDir);
	while ( (retVal = getline (&cmdLine, &len, stdin)) != -1) {
		// remove cmdLine last char (\n)
		cmdLine [strlen(cmdLine)-1] = 0; 
		if (parseAndExecute (cmdLine) == 1) break;
		printf ("%s: ", currDir);
	} 
	free (cmdLine);
	free (disk);
	return 0;
}//main
