/***********************************************************/
/* This source is part of SNEmulDS                         */
/* ------------------------------------------------------- */
/* (c) 1997-1999, 2006 archeide, All rights reserved.      */
/* Free for non-commercial use                             */
/***********************************************************/

#ifndef FS_H_
#define FS_H_

#include "gui.h"

void	FS_init();
t_list 	*FS_getDirectoryList(char *mask);
void	FS_printlog(char *buf);
char	*FS_loadROM(char *filename, int *size);
char	*FS_loadROMForPaging(char *filename, int *size);
int		FS_loadROMPage(char *buf, unsigned int pos, int size);
int		FS_loadFile(char *filename, char *buf, int size);

#endif /*FS_H_*/