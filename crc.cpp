#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <stdlib.h>
#include <cstdio>
#include <iostream>

#define CHAR_SIZE 8
#define CRC_SIZE 12
uint32_t PATTERN = 0x80F; //x11 + x3 + x2 + x + 1

uint32_t crc12(char* data) {
    int len = strlen(data);
    uint32_t remainder = 0;
    uint32_t nremainder = 0;
    for (int i = 0; i < len; i++) {
        for (int j = CHAR_SIZE - 1; j >= 0; j--) {
            bool bit = (data[i] & (1 << j)) > 0;
            bool lb = (remainder & (1 << (CRC_SIZE - 1))) > 0;
            int fb = lb ^ bit;
            nremainder = fb;
            for (int k = 1; k < CRC_SIZE; k++) {
                int cb = (remainder & (1 << (k - 1))) >> k - 1;
                if (PATTERN & (1 << k)) {
                    nremainder |= (cb ^ fb) << k;
                }
                else {
                    nremainder |= cb << k;
                }
            }
            remainder = nremainder;
        }
    }
    return remainder;
}
char* num_to_binary(int num, int size) {
    char* output = (char*)malloc((size + 1) * sizeof(char));
    if (output == NULL)
        return NULL;
    for (int i = 0; i < size; i++) {
        output[size - i - 1] = (num & 1) + '0';
        num >>= 1;
    }
    output[size] = 0;
    return output;
}
char* char_to_binary(char ch) {
    return num_to_binary(ch, CHAR_SIZE);
}

void reversestr(char* source, char target[], unsigned int length)//转换16进制
{
    int i;
    for (i = 0; i < length; i++)
        target[i] = source[length - 1 - i];
    target[i] = 0;
}
void tohex(unsigned long num, char* hexStr)//转换16进制
{
    unsigned long n = num;
    char hextable[] = "0123456789abcdef";
    char temphex[16], hex[16];
    int i = 0;
    while (n) {
        temphex[i++] = hextable[n % 16];
        n /= 16;
    }
    temphex[i] = 0;
    reversestr(temphex, hex, i);
    strcpy(hexStr, hex);
}

void Calc_CRC(char* input)//计算CRC
{
    int len = strlen(input);
    int data = crc12(input);
   /* printf("CRC12 remainder of str \"%s\" is %x\n", input, data);
    printf("The binary string is:\n");*/
    printf("CRC12 remainder of str \"%s\" is %x\n", input, data);
    printf("%s\n", num_to_binary(data, CRC_SIZE));
}

bool is_correct(char* input, double checksum)//检查是否合法
{
    int len = strlen(input);
    char temp[500] = {};
    for (int i = 0; i < len-3 ; i++)
    {
        temp[i] = input[i];
    }
  /*  printf("%d\n", strlen(temp));
    printf("%s\n", (temp));
    printf("%x\n", crc12(temp));*/
    Calc_CRC(temp);
    if (crc12(temp) == checksum)
    {
        printf("ok\n");
        return true;
    }
    else
    {
        printf("wrong\n");
        return false;
    }
    
}
void check_CRC(char* input)//检查是否相同
{
    double sum = 0;
    int j = 0;//指数
    for (int i = strlen(input)-1; i >= strlen(input)-3; i--)
    {
        int temp;
        temp = input[i];


        if(input[i]>='0'&&input[i]<='9')
        sum += (input[i]-'0')*pow(16, j);
        else
        {
            if (input[i] >= 'a' && input[i] <= 'f')
            {
                sum += (input[i] - 'a' + 10) * pow(16, j);
            }
        }
        j++;
    }

    is_correct(input,sum);
}

char* Add_CRC(char* input)//将字符串转为加上CRC的形式
{
    //char input[] = "HelloWorldqwe241432534543634";
    int len = strlen(input);

    char* input1 = (char*)malloc(sizeof(char) * (len - 1));

    for (int i = 0; i < len; i++)
    {
        input1[i] = input[i];
    }
    //printf("%s\n", input1);
    int data = crc12(input);
    //printf("%d\n", strlen(input));
    //printf("CRC12 remainder of str \"%s\" is %x\n", input, data);
    //printf("The binary string is:\n");
    Calc_CRC(input);

    char check[4];

    tohex(data, check);
    printf("%s\n", check);
    
    if (strlen(check) == 1)
    {
        check[2] = check[0];
        check[1] = '0';
        check[0] = '0';
    }
    if (strlen(check) == 2)
    {
        check[2] = check[1];
        check[1] = check[0];
        check[0] = '0';
    }
    for (int i = 0; i < 3; i++)
    {
        input1[len + i] = check[i];
    }
    input1[len + 3] = '\0';

    return input1;
}





int main() {
    char input[] = "123321";
    int len = strlen(input);
    
    check_CRC(Add_CRC(input));
    
}