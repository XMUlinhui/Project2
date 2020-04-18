#include <bits/stdc++.h>
#include <afx.h>
#include <WinSock2.h> 
#pragma warning (disable : 4996)

//本机为服务器，用于传输文件
#define PORT 3214   //netstat -ano查询本机（127.0.0.1）可用端口
#define SERVER_IP "127.0.0.1" //ipconfig查询本机ipv4
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512 
#define CHUNK_SIZE 1024
#pragma comment(lib, "WS2_32") 

int main()
{
	std::string filePath;
	std::cout << "这是客户端，请输入下载文件路径\n";
	std::cin >> filePath;
	CString FilePath(filePath.c_str());

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


		int FileLen = 0;
		int nCurrentPos = 0; //断点位置
		UINT OpenFlags;
		CFile PosFile;
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
			int nChunkCount;
			CFile file;
			nChunkCount = FileLen / CHUNK_SIZE;
			if (FileLen % nChunkCount != 0)
			{
				nChunkCount++;
			}
			if (file.Open((LPCTSTR)FilePath, OpenFlags))
			{
				file.Seek(nCurrentPos * CHUNK_SIZE, CFile::begin);
				char* date = new char[CHUNK_SIZE];
				for (int i = nCurrentPos; i < nChunkCount; i++)
				{
					int nLeft;
					if (i + 1 == nChunkCount)
						nLeft = FileLen - CHUNK_SIZE * (nChunkCount - 1);
					else
						nLeft = CHUNK_SIZE;
					int idx = 0;
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
					int seekpos = i + 1;
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
				std::cout << "文件传输完成";
		}

		WSACleanup();
		return 0;
	}
}