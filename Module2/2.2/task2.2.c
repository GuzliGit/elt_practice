#include "operations.h"
#include <stdio.h>
#include <stdlib.h>

void refresh_disp()
{
    printf("\033[2J\033[H");
}

double input_number() {
    char buffer[100];
    double number;
    
    while (1) 
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) 
        {
            printf("Ошибка ввода!\n");
            continue;
        }
        
        char* endptr;
        number = strtod(buffer, &endptr);
        
        if (endptr == buffer || *endptr != '\n') 
        {
            printf("Введите число!\n");
        } 
        else 
        {
            break;
        }
    }
    
    return number;
}

int main()
{
    int choice = 0;
    char input;
    double x, y, result;
    while (1)
    {
        refresh_disp();
        printf("Калькулятор:\n");
        printf("1. Сложение\n");
        printf("2. Вычитание\n");
        printf("3. Умножение\n");
        printf("4. Деление\n");
        printf("5. Выход\n");

        input = getchar();
        if (input == EOF) 
        {
            clearerr(stdin);
            input = '\0';
            continue;
        }
        while(getchar() != '\n');

        switch(input)
        {
            case '1':
                printf("Введите первое число:\n");
                x = input_number();
                printf("Введите второе число:\n");
                y = input_number();
                printf("Результат: %.2lf\n", sum(x, y));
                printf("Для продолжения нажмите Enter...\n");
                getchar();
                break;
            case '2':
                printf("Введите первое число:\n");
                x = input_number();
                printf("Введите второе число:\n");
                y = input_number();
                printf("Результат: %.2lf\n", substraction(x, y));
                printf("Для продолжения нажмите Enter...\n");
                getchar();
                break;
            case '3':
                printf("Введите первое число:\n");
                x = input_number();
                printf("Введите второе число:\n");
                y = input_number();
                printf("Результат: %.2lf\n", multiplication(x, y));
                printf("Для продолжения нажмите Enter...\n");
                getchar();
                break;
            case '4':
                printf("Введите первое число:\n");
                x = input_number();
                printf("Введите второе число:\n");
                y = input_number();
                if (!division(x, y, &result))
                    printf("Ошибка деления!\n");
                else
                    printf("Результат: %.2lf\n", result);
                printf("Для продолжения нажмите Enter...\n");
                getchar();
                break;
            case '5':
                return 0;
                break;
        }
    }

    return 0;
}