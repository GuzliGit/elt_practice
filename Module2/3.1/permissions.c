#include "permissions.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_sym(mode_t mode)
{
    if (S_ISREG(mode)) printf("-");
    else if (S_ISDIR(mode)) printf("d");
    else if (S_ISBLK(mode)) printf("b");
    else if (S_ISCHR(mode)) printf("c");
    else if (S_ISFIFO(mode)) printf("p");
    else if (S_ISSOCK(mode)) printf("s");
    else if (S_ISLNK(mode)) printf("l");
    else printf("!");

    printf("%c%c%c", 
        (mode & S_IRUSR) ? 'r' : '-', 
        (mode & S_IWUSR) ? 'w' : '-',
        (mode & S_ISUID) ? 's' : ((mode & S_IXUSR) ? 'x' : '-'));

    printf("%c%c%c", 
        (mode & S_IRGRP) ? 'r' : '-', 
        (mode & S_IWGRP) ? 'w' : '-',
        (mode & S_ISGID) ? 's' : ((mode & S_IXGRP) ? 'x' : '-'));

    printf("%c%c%c", 
        (mode & S_IROTH) ? 'r' : '-', 
        (mode & S_IWOTH) ? 'w' : '-',
        (mode & S_ISVTX) ? 't' : ((mode & S_IXOTH) ? 'x' : '-'));
}

void print_dig(mode_t mode)
{
    printf("%o", (mode & 0777));
}

void print_bin(mode_t mode)
{
    for (int i = 8; i >= 0; i--)
    {
        printf("%d", (mode & (1 << i)) ? 1 : 0);
    }
}

int exec_chmod_c(char* filename, int argc, char* argv[])
{
    struct stat filestat;
    if (lstat(filename, &filestat) != 0)
    {
        printf("Такого файла не существует!\n");
        return 0;
    }

    mode_t mode = filestat.st_mode;
    mode_t temp = mode;
    int is_dir = S_ISDIR(mode);
    char sign;
    int main_perms, side_perms;

    printf("Текущие права:\n");
    print_sym(mode);
    printf(" ");
    print_dig(mode);
    printf(" ");
    print_bin(mode);
    printf(" %s", filename);
    printf("\n");

    for (int i = 1; i < argc; i++)
    {
        char* arg = argv[i];

        if (arg[0] >= '0' && arg[0] <= '7')
        {
            temp = (temp & ~07777) | (strtol(arg, NULL, 8) & 07777);
        }
        else
        {
            int j = 0;
            mode_t mask;
            switch (arg[0]) 
            {
                case 'u':
                    j++;
                    mask = 07700;
                    break;
                case 'g':
                    j++;
                    mask = 07070;
                    break;
                case 'o':
                    j++;
                    mask = 07007;
                    break;
                case 'a':
                    j++;
                    mask = 07777;
                    break;
                default:
                    mask = 07777;
                    break;
            }

            if (arg[j] != '+' && arg[j] != '-' && arg[j] != '=')
            {
                printf("Некорректная запись прав доступа!\n");
                return -1;
            }

            main_perms = 0;
            side_perms = 0;
            sign = arg[j];
            j++;

            while (arg[j] != '\0') 
            {
                switch (arg[j]) 
                {
                    case 'r':
                        main_perms |= S_IRUSR | S_IRGRP | S_IROTH;
                        break;
                    case 'w':
                        main_perms |= S_IWUSR | S_IWGRP | S_IWOTH;
                        break;
                    case 'x':
                        main_perms |= S_IXUSR | S_IXGRP | S_IXOTH;
                        break;
                    case 's':
                        if (arg[0] == 'u')
                            side_perms |= S_ISUID;
                        else if (arg[0] == 'g')
                            side_perms |= S_ISGID;
                        else if (arg[0] == 'a' || arg[0] == '+' || arg[0] == '-' || arg[0] == '=')
                            side_perms |= S_ISGID | S_ISUID;
                        break;
                    case 't':
                        if (arg[0] == 'o' || arg[0] == '+' || arg[0] == '-' || arg[0] == '=')
                            side_perms |= S_ISVTX;
                        break;
                    default:
                        printf("Некорректный атрибут прав доступа: %c\n", arg[j]);
                        return -1;
                }
                j++;
            }

            switch (sign) 
            {
                case '+':
                    temp |= (main_perms & mask);
                    temp |= side_perms;
                    break;
                case '-':
                    temp &= ~(main_perms & mask);
                    temp &= ~side_perms;
                    break;
                case '=':
                {
                    mode_t preserve_mask = ~mask;
                    if (arg[0] != 'u' && (arg[0] == 'g' || arg[0] == 'o')) 
                        preserve_mask |= S_ISUID;
                    if (arg[0] != 'g' && (arg[0] == 'u' || arg[0] == 'o')) 
                        preserve_mask |= S_ISGID;
                    if (arg[0] != 'o' && (arg[0] == 'u' || arg[0] == 'g')) 
                        preserve_mask |= S_ISVTX;

                    mode_t preserved_bits = temp & preserve_mask;
                    temp = (temp & ~mask) | (main_perms & mask);
                    temp |= side_perms;
                    temp |= preserved_bits;
                    break;
                }
            }
        }
    }

    if (argc > 1)
    {
        printf("Новые права:\n");
        print_sym(temp);
        printf(" ");
        print_dig(temp);
        printf(" ");
        print_bin(temp);
        printf(" %s", filename);
        printf("\n");
    }

    return 1;
}
