#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int HexToDec(char ch)
{
    int num;
    if (ch >= '0' && ch <= '9')
        num = ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        num = 10 + ch - 'a';
    else if (ch >= 'A' && ch <= 'F')
        num = 10 + ch - 'A';
    return num;
}
int main()
{
    char buffer[10] = {
        0,
    };
    int temp[5] = {
        0,
    };
    FILE *file;
    if (!(file = fopen("inst3.data", "r")))
        return errno;
    while (1)
    {
        if (feof(file))
            break;
        fgets(buffer, sizeof(buffer), file);
        for (int i = 0; i < 2; i++)
        {
            temp[i] = HexToDec(buffer[i]);
        }
        printf("num : %d, %d \n", temp[0], temp[1]);
    }
}