#include <bits/stdc++.h>
#include <afx.h>
#include <WinSock2.h> 
#include<time.h>
#include "unzip.h"
#include "zip.h"
#pragma warning (disable : 4996)

//本机为服务器，用于传输文件
//#define PORT 3215   //netstat -ano查询本机（127.0.0.1）可用端口 默认3215！！
//#define SERVER_IP "127.0.0.1" //ipconfig查询本机ipv4

#define FILE_NAME_MAX_SIZE 512 
#define CHUNK_SIZE 1024
#pragma comment(lib, "WS2_32") 


int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("usage : %s filename ip and port\n", argv[0]);
		exit(1);
	}
	USHORT PORT = atoi(argv[3]);
	int len1 = strlen(argv[2]);
	int len2 = strlen(argv[1]);

	//char* SERVER_IP = new char(len1);
	//char* FilePath = new char(len2 + 10);

	char FilePath[FILE_NAME_MAX_SIZE];
	char filePath[FILE_NAME_MAX_SIZE];
	char SERVER_IP[FILE_NAME_MAX_SIZE];
	strcpy(FilePath, argv[1]);
	strcpy(SERVER_IP, argv[2]);
	strcpy(filePath, FilePath);


	int zipLength = strlen(FilePath);
	if (FilePath[zipLength - 1] != '/')
	{
		char bac[] = "/abc.zip";
		strcat(FilePath, bac);
	}
	else
	{
		char bac[] = "abc.zip";
		strcat(FilePath, bac);
	}

	printf("%s\n%s\n", FilePath, SERVER_IP);

	std::string TMPF = "PosFile.temp";
	CString TMPFile(TMPF.c_str());

	// 初始化socket dll 
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 0);
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{

		printf("错误：Socket初始化失败!");
		exit(1);
	}

	//std::cout << "这是客户端\n";
	while (1)
	{
		//创建socket 
		SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0);
		if (SOCKET_ERROR == c_Socket)
		{
			printf("错误：创建Socket异常!");
			exit(1);
		}

		//指定服务端的地址 
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
		server_addr.sin_port = htons(PORT);


		if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
		{
			
			printf("错误：无法连接到服务端IP！\n");
			exit(1);
		}

		printf("CONNECT\n");
		long long FileLen = 0;
		long long nCurrentPos = 0; //断点位置
		UINT OpenFlags;
		CFile PosFile;

		clock_t start_time = clock();

		if (PosFile.Open(TMPFile, CFile::modeRead | CFile::typeBinary))//如果有临时文件则读取断点
		{
			PosFile.Read((char*)&nCurrentPos, sizeof(nCurrentPos)); //读取断点位置
			std::cout << "文件位置在 " << nCurrentPos << "\n";
			nCurrentPos = nCurrentPos + 1;
			PosFile.Close();
			send(c_Socket, (char*)&nCurrentPos, sizeof(nCurrentPos), 0); //发送断点值
			OpenFlags = CFile::modeWrite | CFile::typeBinary;
		}
		else
		{
			send(c_Socket, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
			OpenFlags = CFile::modeWrite | CFile::typeBinary | CFile::modeCreate;
		}

		if (recv(c_Socket, (char*)&FileLen, sizeof(FileLen), 0) != 0)
		{
			
			long long nChunkCount = (FileLen + CHUNK_SIZE - 1) / CHUNK_SIZE;
			CFile file;


			if (file.Open((LPCTSTR)FilePath, OpenFlags))
			{
				file.Seek((long long)nCurrentPos * CHUNK_SIZE, CFile::begin);
				char* date = new char[CHUNK_SIZE];
				for (long long i = nCurrentPos; i < nChunkCount; i++)
				{
					long long nLeft;
					if (i + 1 == nChunkCount)
						nLeft = FileLen - CHUNK_SIZE * (nChunkCount - 1);
					else
						nLeft = CHUNK_SIZE;
					long long idx = 0;
					while (nLeft > 0)
					{
						int ret = recv(c_Socket, &date[idx], nLeft, 0);
						if (ret == SOCKET_ERROR)
						{
							std::cout << "文件传输异常";
							return 0;
						}
						idx += ret;
						nLeft -= ret;
					}
					file.Write(date, CHUNK_SIZE);


					CFile PosFile; //将断点写入PosFile.temp文件
					long long seekpos = i + 1;
					if (PosFile.Open((LPCTSTR)TMPFile, CFile::modeWrite | CFile::typeBinary | CFile::modeCreate));
					{
						PosFile.Write((char*)&seekpos, sizeof(seekpos));
						PosFile.Close();
					}
				}
				file.Close();
				delete[] date;
			}


			if (DeleteFile((LPCTSTR)TMPFile) != 0)
			{
				std::cout << "文件传输完成"<<std::endl;
				
				clock_t end_time = clock();
				double Bps = FileLen / (double)(end_time - start_time);
				double  tr = 1048576;
				double MBs = Bps / tr*1000;
				std::cout <<MBs <<" MB/s"<< std::endl;
				
				CString strRarPath = "\"WinRAR\\WinRAR.exe\" x -iext -ow -ver -- ";
				CString strfinal = strRarPath + FilePath +" "+filePath;
				printf("%s\n", strfinal);
				system(strfinal);

				std::cout << "解压完成" << std::endl;
				remove(FilePath);
				
			}
		}

		WSACleanup();
		return 0;
	}
}