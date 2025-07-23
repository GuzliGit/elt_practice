#include "subnet.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Некорректный запуск!\nТребуемый формат: [исполняемый файл] [шлюз] [маска подсети] [кол-во пакетов].\n");
        return -1;
    }

    start_sending_sim(argv + 1);

    return 0;
}