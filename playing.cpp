#include <iostream>
#include <random>  
#include <map>
#include <stdio.h>
#include <ctime> 
#include <functional>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;
int h; // счестчик для синхронизации массива N и потоков
std::mutex g_lock;
int N[22]; // аккумулятор

map <int, int> distribution = {{0, 0}, // таблица призов
                               {1, 1},
                               {2, 2},
                               {3, 3},
                               {4, 5},
                               {5, 7},
                               {6, 10},
                               {7, 50},
                               {8, 100},
                               {9, 200},
                               {10, 500} };

map <int, int> ::iterator it;

void playing() { // разыгрывание случайной величины

    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d({ 85, 3.10585679377601, 5.94949346211722, 0.306384154012617,
        2.62116655049198, 2.49297402533018, 0.233545647387999, 0.142275767233313, 0.0494024168152975,
        0.0494024168152975, 0.0494987660200913 }) ; //вычисленные в excel значения распределения случайной величины

    std::map<int, int> m;
    for (int n = 0; n < 10000; ++n) {
        ++m[d(gen)];
    }

    g_lock.lock();
    it = distribution.begin();
    int i = h;
     for (auto p : m) {
         N[i] = p.second;
         it++;
         i++;
     }

     h = 11;
     g_lock.unlock();
}

void calc(int* N, double*P, double M, double D) { 

    int x, n;
    it = distribution.begin();
   
    for (int i = 0; i < 11; i++) {
        P[i] = (double) N[i] / 20000;
        M += P[i] * it->second; // вычисление мат. ожидания
        it++;
    }

    D = 0;
    it = distribution.begin();

    for (int i = 0; i < 11; i++) {
        D += (it->second - M) * (it->second - M) * P[i]; // вычисление дисперсии
        it++;
    }
    cout << "M: " << M <<endl << "D: " << D << endl;
}

void printArr(int* Arr) { // вывод сгенерированных данных в файл

    ofstream fout("data.txt");
    it = distribution.begin();

    for (int i = 0; i < 11; i++) {
        fout << "times: " << Arr[i] << "\t value: " <<it->second << endl;
        it++;
    } 
}

void dataIn() { // запись данных в хранилище длиною в 11

    for (int i = 0; i < 11; i++) {
        N[i] += N[i + 11];
    } 
}

int main()
{
    h = 0;
    srand((unsigned int)time(0));
       
    std::thread t1(playing);
    std::thread t2(playing);
    t1.join();
    t2.join();

    dataIn();

    double M = 0, D = 0, P[11];
 
    calc(N, P, M, D);

    printArr(N);

    return 0;
}
   

