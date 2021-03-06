/** @file src/file.h %File access definitions. */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include "types.h"

enum
{
	FILE_MODE_READ = 0x01,
	FILE_MODE_WRITE = 0x02,
	FILE_MODE_READ_WRITE = FILE_MODE_READ | FILE_MODE_WRITE,

	FILEINFO_MAX = 686,
	FILEINFO_INVALID = 0xFFFF,

	FILE_MAX = 20,
	FILE_INVALID = 0xFF
};

enum SearchDirectory
{
	SEARCHDIR_ABSOLUTE,
	SEARCHDIR_DATA_DIR,
	SEARCHDIR_CAMPAIGN_DIR,
	SEARCHDIR_SCENARIO_DIR,
	SEARCHDIR_SAVE_DIR
};

/**
 * Static information about files and their location.
 */
struct FileInfo
{
	const char* filename; /*!< Name of the file. */
	uint32 fileSize; /*!< The size of this file. */
	uint32 filePosition; /*!< Where in the file we currently are (doesn't have to start at zero when in PAK file). */
	uint16 parentIndex; /*!< In which FileInfo this file can be found. */
	struct
	{
		BIT_U8 isLoaded:1; /*!< File is mapped in the memory. */
		BIT_U8 inPAKFile:1; /*!< File can be in other PAK file. */
	} flags; /*!< General flags of the FileInfo. */
};

extern char g_dune_data_dir[1024];

void FileHash_Init();
extern FileInfo* FileHash_Store(const char* key);
extern unsigned int FileHash_FindIndex(const char* key);

void File_MakeCompleteFilename(char* buf, size_t len, SearchDirectory dir, const char* filename, bool convert_to_lowercase);
FILE* File_Open_CaseInsensitive(SearchDirectory dir, const char* filename, const char* mode);
void File_Close(uint8 index);
uint32 File_Read(uint8 index, void* buffer, uint32 length);
uint16 File_Read_LE16(uint8 index);
uint32 File_Read_LE32(uint8 index);
uint32 File_Write(uint8 index, void* buffer, uint32 length);
uint32 File_Seek(uint8 index, uint32 position, uint8 mode);
uint32 File_GetSize(uint8 index);
uint16* File_ReadWholeFileLE16(const char* filename);
void ChunkFile_Close(uint8 index);
uint32 ChunkFile_Seek(uint8 index, uint32 header);
uint32 ChunkFile_Read(uint8 index, uint32 header, void* buffer, uint32 buflen);

#define File_Exists(FILENAME)               File_Exists_Ex(SEARCHDIR_DATA_DIR,   FILENAME)
#define File_Open(FILENAME,MODE)            File_Open_Ex(SEARCHDIR_DATA_DIR,   FILENAME, MODE)
#define File_ReadBlockFile(FILENAME,BUFFER,LENGTH)          File_ReadBlockFile_Ex(SEARCHDIR_DATA_DIR,   FILENAME, BUFFER, LENGTH)
#define File_ReadWholeFile(FILENAME)        File_ReadWholeFile_Ex(SEARCHDIR_DATA_DIR, FILENAME)
#define ChunkFile_Open(FILENAME)            ChunkFile_Open_Ex(SEARCHDIR_DATA_DIR,   FILENAME)

bool File_Exists_Ex(SearchDirectory dir, const char* filename);
uint8 File_Open_Ex(SearchDirectory dir, const char* filename, uint8 mode);
uint32 File_ReadBlockFile_Ex(SearchDirectory dir, const char* filename, void* buffer, uint32 length);
void* File_ReadWholeFile_Ex(SearchDirectory dir, const char* filename);
uint32 File_ReadFile_Ex(SearchDirectory dir, const char* filename, void* buf);
uint8 ChunkFile_Open_Ex(SearchDirectory dir, const char* filename);
void File_Delete_Ex(SearchDirectory dir, const char* filename);

bool fread_le_uint32(uint32* value, FILE* stream);
bool fread_le_uint16(uint16* value, FILE* stream);
bool fwrite_le_uint32(uint32 value, FILE* stream);
bool fwrite_le_uint16(uint16 value, FILE* stream);

#define fread_le_int32(p, f) fread_le_uint32((uint32 *)(p), (f))
#define fread_le_int16(p, f) fread_le_uint16((uint16 *)(p), (f))
#define fwrite_le_int32(v, f) fwrite_le_uint32((uint32)(v), (f))
#define fwrite_le_int16(v, f) fwrite_le_uint16((uint16)(v), (f))

#endif /* FILE_H */
