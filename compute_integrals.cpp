#include <mpi.h>
#include <iostream>
#include <cmath>
#include <string>


class ComputeIntegrals{
/* Численно считает  определенный интеграл
 
    input:
 - подынтегральная функция из предложенных: x, x^2, sin x, cos x
 - метод интегрирования: трапецией или прямоугольником
 - границы отрезка интегрирования (начало и конец)
 - число отрезков разбиения при интегрировании
 
    return:
 значение интеграла функции на промежутке
 */
    
public:
    // Параметры интегрирования
    
    int function_mode = 0; // Подынтегральная функция
    std::string integrate_mode = "trapeze"; // Метод интегрирования
    int steps_number = 1001; // Число отрезков разбиения при интегрировании
    float start_int = 0; // Координата начала отрезка интегрирования
    float end_int = 1; // Координата конца отрезка интегрирования
        
    int menu(){
        // Меню действий для пользователя
        
        std::cout << "This is an instrument to calculate definite integrals.\n";
        std::cout << "Default parameters: function = x, method = trapeze, interval = [0,1], number of partition segments = 1001. \n";
        std::cout << "If you want to choose another function, press 1.\n";
        std::cout << "If you want to choose another method, press 2.\n";
        std::cout << "If you want to choose another interval, press 3.\n";
        std::cout << "If you want to choose another number of partition segments, press 4.\n";
        std::cout << "If you chose parameters and want to integrate, press 0.\n";
        // Пример ввода: 1
        
        // Выбор параметра пользователем
        std::string input;
        int choice;
        try {
            try {
                std::cin >> input;
                if (input.find_first_not_of("01234") != std::string::npos)
                    throw "Please, enter just digits.\n";
                choice = std::stoi(input);
            }
            catch (const char* exception){
                std::cerr << "Invalid input: " << exception << '\n';
            }
            if (choice > 4 || choice < 0)
                throw "Please, choose a number from 0 to 4.";
            return choice;
        }
        catch (const char* exception){
            std::cerr << "Invalid input: " << exception << '\n';
        }
    }
    
    void read_input(int rank){
        // Считывание задаваемых параметров
        
        std::string input, second_input;
        int choice = 1;
        int err;
        if (rank == 0){ // Считывание для процесса с рангом 0
            while (choice > 0){
            // Выход из меню, если пользователь уже выбрал все параметры
                choice = menu();
                switch (choice) {
                    case 1:
                        std::cout << "If you want to integrate a function x, press 0, x^2 - press 1, sin x - press 2, cos x - press 3.\n";
                        // Пример ввода: 2
                        try {
                            try {
                                std::cin >> input;
                                if (input.find_first_not_of("0123") != std::string::npos)
                                    throw "Please, enter just digits.\n";
                                function_mode = std::stoi(input);
                            }
                            catch (const char* exception){
                                std::cerr << "Invalid input: " << exception << '\n';
                            }
                            if (function_mode < 0 || function_mode > 3)
                                throw "Please, choose a number from 0 to 3.";
                        }
                        catch (const char* exception){
                            std::cerr << "Invalid input: " << exception << '\n';
                        }
                        break;
            
                    case 2:
                        std::cout << "Enter a method: rectangle or trapeze.\n";
                        // Пример ввода: rectangle
                        try {
                            std::cin >> input;
                            if (input != "rectangle" || "trapeze")
                                throw "Please, choose a method from these: rectangle or trapeze. \n";
                            integrate_mode = input;
                        }
                        catch (const char* exception){
                            std::cerr << "Invalid input: " << exception << '\n';
                        }
                        break;
            
                    case 3:
                        std::cout << "Enter the interval.\n";
                        // Пример ввода: 3 6
                        try {
                            std::cin >> input >> second_input;
                            if (input.find_first_not_of("0123456789.-") != std::string::npos || second_input.find_first_not_of("0123456789.-") != std::string::npos)
                                throw "Please, enter just digits.\n";
                            start_int = std::stof(input);
                            end_int = std::stof(second_input);
                        }
                        catch (const char* exception){
                            std::cerr << "Invalid input: " << exception << '\n';
                        }
                        break;
            
                    case 4:
                        std::cout << "Enter the number of partition segments.\n";
                        // Пример ввода: 67
                        try {
                            std::cin >> input;
                            if (input.find_first_not_of("0123456789") != std::string::npos)
                                throw "Please, enter just digits.\n";
                            steps_number = std::stoi(input);
                        }
                        catch (const char* exception){
                            std::cerr << "Invalid input: " << exception << '\n';
                        }
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        // Передача параметров остальным процессам
        err = MPI_Bcast(&start_int, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        err = MPI_Bcast(&end_int, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
        err = MPI_Bcast(&steps_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
        err = MPI_Bcast(&function_mode, 1, MPI_INT, 0, MPI_COMM_WORLD);
        err = MPI_Bcast(&integrate_mode, 8, MPI_CHAR, 0, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS)
               MPI_Abort(MPI_COMM_WORLD, 4);
    }
    
    float current_function(float x){
        // Задаваемая для интегрирования функция
        switch (function_mode) {
            case 1:
                return pow(x, 2);
            case 2:
                return sin(x);
            case 3:
                return cos(x);
            default:
                return x;
        }
    }
    
    void compute(int rank, int size){
        // Подсчет интеграла
        
        float h = (end_int - start_int) / steps_number; // Длина отрезка разбиения
        float result;
        if (integrate_mode == "trapeze")
            result = (current_function(start_int) + current_function(end_int)) * h / 2;
        else
            result = current_function(start_int) * h;
        int n0 = steps_number - 1; // Число точек
       
        int points_for_proc = n0 / size; // Число точек, обрабатывающихся за процесс
        int iterate = n0 / size;
        if (rank == size - 1)
            iterate += n0 % size;
        int max_points_in_rank = n0 / size + n0 % size;
        float* coeff = new float[max_points_in_rank]; // Коэффициенты, найденные за rank
        
        for (int i = 0; i < max_points_in_rank; i++){
            if (i < iterate)
                coeff[i] = current_function(start_int + (rank * points_for_proc + i + 1) * h) * h;
            else
                coeff[i] = 0;
        }
        
        float* rec_buf; // Массив, принимающий коэффициенты от всех процессов
        if (rank == 0)
            rec_buf = new float [max_points_in_rank];

        int err = MPI_Reduce(coeff, rec_buf, max_points_in_rank, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS)
            MPI_Abort(MPI_COMM_WORLD, 5);

        if (rank == 0){
            for (int j = 0; j < max_points_in_rank; j++)
                result += rec_buf[j];

            delete[] rec_buf;
             std::cout << "Integral calculated = " << result << std::endl; // Вывод результата
        }
    }
};

int main(int argc, char * argv[]){
    ComputeIntegrals integral;
        
    int rank, size;
    int err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS)
        MPI_Abort(MPI_COMM_WORLD, 1);
    if (MPI_Comm_size(MPI_COMM_WORLD, &size) != MPI_SUCCESS)
        MPI_Abort(MPI_COMM_WORLD, 2);
    if (MPI_Comm_rank(MPI_COMM_WORLD, &rank) != MPI_SUCCESS)
        MPI_Abort(MPI_COMM_WORLD, 3);
    
    // Считывание входных данных
    integral.read_input(rank);

    // Подсчет интеграла
    integral.compute(rank, size);
    
    err = MPI_Finalize();
    if (err != MPI_SUCCESS)
        MPI_Abort(MPI_COMM_WORLD, 5);
    
    return 0;
}
