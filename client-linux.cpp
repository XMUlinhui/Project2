#include "head.h"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("usage : %s ip filename and format\n", argv[0]);
        exit(1);
    }
    
    char* IP = argv[2];
    unsigned short PORT = atoi(argv[3]);

    int sockfd;
    struct sockaddr_in c_addr;
    int c_addr_len = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) strerr("socket error");
    memset(&c_addr, 0, c_addr_len);

    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = inet_addr(argv[2]);
    c_addr.sin_port = htons(PORT);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (connect(sockfd, (struct sockaddr*) & c_addr, c_addr_len) == -1)
        strerr("connect error");


    long long FileLen = 0;
    long long nCurrentPos = 0;
    std::string tmp = argv[1];
    tmp += "/abc.zip";

        FILE* tfp = fopen("PosFile.temp", "rb");
        if (tfp != NULL)
        {
            fread((char*)&nCurrentPos, sizeof(nCurrentPos), 1, tfp);
            nCurrentPos &= 0;
            fclose(tfp);
            send(sockfd, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
        }
        else
        {
            send(sockfd, (char*)&nCurrentPos, sizeof(nCurrentPos), 0);
        }
        


        if (recv(sockfd, (char*)&FileLen, sizeof(FileLen), 0) != 0)
        {
            long long nChunkCount = (FileLen + CHUNK_SIZE - 1) / CHUNK_SIZE;

            FILE* fp = fopen(tmp.c_str(), "wb");
            if (fp == NULL) strerr("can not create file");
            else
            {
                fseek(fp, (long long)nCurrentPos * CHUNK_SIZE, 0);
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
                        long long ret = recv(sockfd, &date[idx], nLeft, 0);
                        if (ret == -1)
                        {
                            printf("file trans error");
                            return 0;
                        }
                        idx += ret;
                        nLeft -= ret;
                    }
                    if (fwrite(date, sizeof(char), idx, fp) < idx)
                        strerr("fwrite error");


                    FILE* PosFile = fopen("PosFile.temp", "wb"); //将断点写入PosFile.temp文件
                    long long seekpos = i + 1;
                    if (PosFile != NULL);
                    {
                        long long tlenth = strlen((char*)&seekpos);
                        fwrite((char*)&seekpos, sizeof(char), tlenth + 1, PosFile);
                        fclose(PosFile);
                    }
                
                }
                fclose(fp);
                delete[] date;
            }
            remove("PosFile.temp");

            std::string command = "unzip -O CP936 -o -d ";
            command += argv[1];
            command += " ";
            command += argv[1]; 
            command += "/abc.zip";
            std::cout << command << std::endl;
            system(command.c_str());



            printf("FILE TRANS SUCCESSFULLY!\n");

            std::string removepath = argv[1];
            removepath += "/abc.zip";

            remove(removepath.c_str());


        }



    close(sockfd);
    return 0;
}
