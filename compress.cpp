// compress.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "zip.h"
#include "unzip.h"
using namespace std;
void show_file(char path[],char root[], HZIP hz,int level = 0)
{
    char find_path[128];
    char dir_path[128];
    sprintf(find_path, "%s*", path);
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    bool bContinue = true;
    hFind = FindFirstFile(find_path, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;
    while (bContinue)
    {
        if (_stricmp(FindFileData.cFileName, "..") && _stricmp(FindFileData.cFileName, "."))
        {
            char* name = (char*)malloc(strlen(path) + strlen(FindFileData.cFileName));
            
            //cout << name<<endl;
            //cout <<FindFileData.cFileName << endl;
            for (int i = strlen(root); i < strlen(path); i++)
            {
                dir_path[i - strlen(root)] = path[i];
            }
            dir_path[strlen(path) - strlen(root)] = '\0';
            cout << dir_path << endl;
            
            sprintf(name, "%s%s", path, FindFileData.cFileName);
            char* out_path = (char*)malloc(strlen(dir_path) + strlen(FindFileData.cFileName));
            sprintf(out_path, "%s%s", dir_path, FindFileData.cFileName);
            //cout << name << endl;
            ZipAdd(hz, out_path, name);

            if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
            {
                //cout<<path<<FindFileData.cFileName<<endl;
                sprintf(find_path, "%s%s\\", path, FindFileData.cFileName);
                show_file(find_path, root,hz,level + 1);
            }
        }
        bContinue = FindNextFile(hFind, &FindFileData);
    }
}
//int main()
//{
//    char szPath[] = "c:\\windows\\";
//    show_file(szPath);
//    return 0;
//}

//void CreateFiles();
int main()
{
    //CreateFiles();
    //HZIP hz; DWORD writ;
    //ZIP
    char szPath[] = "G:\\CN-project2\\compress (2)\\compress\\compress\\test\\";
    //char szRoot[] = "G:\\CN-project2\\compress (2)\\compress\\compress\\test\\";
    HZIP hz = CreateZip("test.zip", 0);
    show_file(szPath,szPath,hz);
    
    //HZIP hz = CreateZip("test.zip", 0);
    //ZipAdd(hz, "zntest.txt", "test.txt");
    //ZipAdd(hz, "test", "test");
    //ZipAdd(hz, "znabc/zntt.txt", "abc/tt.txt");
    CloseZip(hz);
    return 0;
    //UNZIP
    //HZIP hz = OpenZip(_T("simple.zip"), 0);
    //SetUnzipBaseDir(hz, _T("\\"));
    //ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
    //for (int zi = 0; zi < numitems; zi++)
    //{
    //    GetZipItem(hz, zi, &ze);
    //    UnzipItem(hz, zi, ze.name);
    //}
    //CloseZip(hz);
    
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
