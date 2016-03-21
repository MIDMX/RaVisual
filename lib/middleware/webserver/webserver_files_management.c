#include <stdio.h>
#include <stdlib.h>
#include "ff.h"
#include "fs.h"
#include "macro_types.h"
//#include "tm_stm32f4_ethernet.h"

//ATTENTION : 2 INSTANCES DE CE MEME DEFINE EXISTENT.... FAITES UNE RECHERCHE EN CAS DE MODIF...
#define ETHERNET_MAX_OPEN_FILES     		10
/* File variable */
static FIL fil[ETHERNET_MAX_OPEN_FILES];
/* Fatfs variable */
FATFS fs;

static bool_e file_system_mounted = FALSE;

int TM_ETHERNETSERVER_OpenFileCallback(struct fs_file* file, const char* name) {
	FRESULT fres;

	char buffer[100];

	/* Print which file you will try to open */
	printf("Trying to open file %s\n", name);

	/* Mount card, it will be mounted when needed */
	if(!file_system_mounted)
	{
		if ((fres = f_mount(&fs, "0:", 1)) == FR_OK)
		{
			file_system_mounted = TRUE;
			printf("SD Card mounted\n");
		}
		else
			printf("Mount error - (No SD Card?) : ERR%d\n", fres);
	}

	/* Format name, I have on subfolder everything on my SD card */
	sprintf((char *)buffer, "0:%s", name);

	/* Try to open */
	fres = f_open(&fil[file->id], buffer, FA_OPEN_EXISTING | FA_READ | FA_WRITE);

	/* If not opened OK */
	if (fres != FR_OK) {
		/* In case we are only opened file, but we didn't succedded */
		if (*file->opened_files_count == 0) {
			/* Unmount card, for safety reason */
			f_mount(NULL, "0:", 1);
			file_system_mounted = FALSE;
		}

		/* Return 0, opening error */
		return 0;
	}

	/* !IMPORTANT; Set file size */
	file->len = f_size(&fil[file->id]);

	/* Return 1, file opened OK */
	return 1;
}

int TM_ETHERNETSERVER_ReadFileCallback(struct fs_file* file, char* buffer, int count) {
	UINT readed;

	/* print debug */
	printf("Trying to read %d bytes from file %s\n", count, file->file_name);

	/* End of file? */
	if (f_eof(&fil[file->id])) {
		return -1;
	}

	/* Read max block */
	if (count > 65535) {
		count = 65535;
	}

	/* Read data */
	f_read(&fil[file->id], (void*)buffer, count, &readed);

	/* Return number of bytes read */
	return readed;
}

void TM_ETHERNETSERVER_CloseFileCallback(struct fs_file* file) {
	/* Close file */
	f_close(&fil[file->id]);

	/* Print to user */
	printf("Closing file %s\n", file->file_name);

	/* Unmount in case there is no opened files anymore */
	if (!*file->opened_files_count) {
		/* Unmount, protect SD card */
		f_mount(NULL, "0:", 1);
		file_system_mounted = FALSE;
		printf("SD Card unmounted\n");
	}
}



