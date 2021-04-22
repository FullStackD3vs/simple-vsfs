#ifndef VSFS_H
#define VSFS_H

#define MAGICNB				0x56534653
#define FT_DIR 				0		// File is a directory
#define FT_FIL				1		// File is a data file
#define TRUE					1
#define FALSE					0

// Any of these parameters can be modified in parameter file.
typedef struct Parameters {
	int blockSize;
	int iNodeBMSize;
	int dataBMSize;
	int iNodeTabSize;
	int directCnt;
} Parameters;

	Parameters parameters;
	int paramDebug;

// Default Values
#define BLOCKSIZE			96		    // bytes
#define INODEBMSIZE			1			// Inode bit map size in block
#define DATABMSIZE			1			// Data bit map size in block
#define INODETABSIZE		10		    // Inode table size in block
#define DIRECTCNT			3			// number of pointers in an iNode
#define FILENAME_LENGTH	    16		    // Max chars in a file name

// The following for shell simulation
#define CMDE_LENGTH		64			// Max length of command
#define CMDCNT				13			// Number of commands
#define BADCMDE 			-1
#define HELP					0
#define LS 						1
#define CD 						2
#define MAK						3
#define MKD						4
#define RMF						5
#define RMD						6
#define CLEAR					7
#define QUIT					99

typedef struct Symbol {
	char *cmd;
	int	val;
} Symbol;

static Symbol lookupTable [CMDCNT] = {
 {"help", HELP},  {"ls", LS}, {"cd", CD}, {"make", MAK}, {"mkdir", MKD}, {"rmf", RMF}, {"rmd", RMD}, {"q", QUIT}, {"clear", CLEAR}
};


typedef struct SuperBlock {
	uint32_t signature;		// magic number
	uint32_t blockCnt;		// blocks on the disk
	uint32_t blockSize;		// in bytes
	uint32_t iNodeBMSize;	// in blocks	
	uint32_t dataBMSize;	// in blocks	
	uint32_t iNodeTabSize;	// in blocks	
	uint32_t iNodeSize;		// in bytes
} SuperBlock;

typedef struct Inode {
	uint8_t type;										// file type
	uint32_t number;								// iNode number
	uint32_t ptr[DIRECTCNT]; 				// pointers to data block
}Inode;



typedef struct DirEntry {
  struct DirEntry *next;
  uint32_t iNodeNb; //inode number	// -1 <=> entry available
  char fileName[FILENAME_LENGTH];
} DirEntry;

// Reading parameters values
void getParams (char *);					// get parameters from file
void initParams ();								// assign default values

// Dumping data.
void dumpBM (uint8_t *);					// dump bit map given its address 
void dumpDataBM ();								// dump data bit map
void dumpInodesBM ();							// dump iNodes bit map
void dumpInode (uint32_t);				// dump iNode (iNode number)
void dumpDataDirBlock (uint32_t);	// dump data block viewed as dir (block number)
void dumpDataBlock (uint32_t);		// dump data block (pure data)
void dumpDisk ();									// dump super block

//API
void vsfs_initDisk (uint32_t);							// disk initialization
void vsfs_mount ();										// mount disk
void vsfs_LS ();										// list files in current directory
void vsfs_CD (char *);								    // change directory
uint32_t vsfs_create (char *, uint8_t);					// create a file in the current directory ot a certain type
uint32_t vsfs_RMD (char *);							    // remove directory in current directory
uint32_t vsfs_RMF (char *);							    // remove file in current directory
Inode *getInode(uint32_t);

#endif
