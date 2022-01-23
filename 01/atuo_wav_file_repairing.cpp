#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#ifndef byte
#define byte  unsigned char
#endif
 

#define BUF_SIZE	256
using namespace std;

int adustHeader(std::string newPath);

int getFileSize(FILE* inFile);
void traverseDir(char* pPath, int nDeepth);

int main(int argc, char** argv)
{
        printf("\n**************************************************************\n");
          printf("*            auto- wav file repairing                        *");
        printf("\n**************************************************************\n");
        printf("\n");
	if(argc < 2)
	{
		printf("usage :\n\n./adjst  auto/20210113   auto/20211014   auto/20211215 \n");
        printf("\n\n");
		exit(1);
	}
    for (int i = 1; i < argc; ++i)
    {
        traverseDir(argv[i], 0);
    }

    return 0;
}

void traverseDir(char* pPath, int nDeepth)
{
    DIR* pDir = NULL;
    struct dirent* pSTDirEntry;
    char* pChild = NULL;


    if ((pDir = opendir(pPath)) == NULL)
    {
        return;
    }
  
	while ((pSTDirEntry = readdir(pDir)) != NULL)
	{
		if ((strcmp(pSTDirEntry->d_name, ".") == 0) || (strcmp(pSTDirEntry->d_name, "..") == 0))
		{
			continue;
		}
		else
		{
			for (int i = 0; i < nDeepth; i++)
			{
				//cout << "\t";
				printf("\t");
			}

			
			int wav_name_len = strlen(pSTDirEntry->d_name);
			if(strcmp(pSTDirEntry->d_name + wav_name_len -4,".wav") == 0)
			{
				char newPath[255];

				sprintf(newPath,"%s/%s/%s",get_current_dir_name(),pPath,pSTDirEntry->d_name);
				printf("%s\n",newPath);
				adustHeader(newPath);

			}

			if (pSTDirEntry->d_type & DT_DIR)
			{
				pChild = (char*)malloc(sizeof(char) * (NAME_MAX + 1));
				if (pChild == NULL)
				{
					perror("memory not enough.");
					return;
				}
				memset(pChild, 0, NAME_MAX + 1);
				strcpy(pChild, pPath);
				strcat(pChild, pSTDirEntry->d_name);
				traverseDir(pChild, nDeepth + 1);
				free(pChild);
				pChild = NULL;
			}
		}
	}
	closedir(pDir);
    
}



int adustHeader(std::string newPath)
{
	FILE* inFile, * outFile;
	char StrLine[BUF_SIZE];

	byte* header = (byte*)malloc(59);


	if ((inFile = fopen(newPath.c_str(), "rb")) == NULL)
	{
		printf("cannot oepn file \n");
		exit(1);
	}
    
    int fSize = getFileSize(inFile);
	int totalDataLen = fSize - 8;
	int dataLen = fSize - 56;

    if(dataLen < 64000) //小于8秒的就算了
        return 0;

	newPath += ".1.wav";
	if ((outFile = fopen(newPath.c_str(), "wb")) == NULL )
	{
		perror("write file failed");
		exit(2);
	}

	
	size_t n56 = fread(header, 1, 56, inFile);
	if (n56 != 56)
	{
		perror("Read file error!");
		exit(-1);
	}
	header[0] = 'R';
	header[1] = 'I';
	header[2] = 'F';
	header[3] = 'F';
	header[4] = (byte)(totalDataLen & 0xff);
	header[5] = (byte)(totalDataLen >> 8 & 0xff);
	header[6] = (byte)(totalDataLen >> 16 & 0xff);
	header[7] = (byte)(totalDataLen >> 24 & 0xff);
	header[8] = 'W';
	header[9] = 'A';
	header[10] = 'V';
	header[11] = 'E';
	header[12] = 'f';
	header[13] = 'm';
	header[14] = 't';
	header[15] = ' ';
	header[16] = 18;
	header[17] = 0;
	header[18] = 0;
	header[19] = 0;
	header[20] = 6;
	header[21] = 0;
	header[22] = 1;
	header[23] = 0;
	header[24] = (byte)(8000 & 0xff);
	header[25] = (byte)((8000 >> 8) & 0xff);
	header[26] = (byte)((8000 >> 16) & 0xff);
	header[27] = (byte)((8000 >> 24) & 0xff);
	header[28] = (byte)(8000 & 0xff);
	header[28] = (byte)((8000 >> 8) & 0xff);
	header[30] = (byte)((8000 >> 16) & 0xff);
	header[31] = (byte)((8000 >> 24) & 0xff);
	header[32] = 1;
	header[33] = 0;
	header[34] = 8;
	header[35] = 0;
	header[36] = 0;
	header[37] = 0;
	header[38] = 'f';
	header[39] = 'a';
	header[40] = 'c';
	header[41] = 't';
	header[42] = 4;
	header[43] = 0;
	header[44] = 0;
	header[45] = 0;
	header[46] = (byte)(dataLen & 0xff);
	header[47] = (byte)(dataLen >> 8 & 0xff);
	header[48] = (byte)(dataLen >> 16 & 0xff);
	header[49] = (byte)(dataLen >> 24 & 0xff);
	header[50] = 'd';
	header[51] = 'a';
	header[52] = 't';
	header[53] = 'a';
	header[54] = (byte)(dataLen & 0xff);
	header[55] = (byte)(dataLen >> 8 & 0xff);
	header[56] = (byte)(dataLen >> 16 & 0xff);
	header[57] = (byte)(dataLen >> 24 & 0xff);
	header[58] = '\0';
	size_t  nr = fwrite(header, 1, 58, outFile);
	if (nr != 58)
	{
		perror("write file failed");
		exit(-2);
	}
	while (!feof(inFile))
	{
		size_t n = fread(StrLine, 1, BUF_SIZE - 1, inFile);
		fwrite(StrLine, 1, n, outFile);
	}

	fclose(outFile);
	fclose(inFile);
}


int getFileSize(FILE* inFile)
{
	int fileSize = 0;
	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);
	return fileSize;
}

