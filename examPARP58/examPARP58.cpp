#include <iostream>
#include <vector>
#include <utility>
#include <chrono>
#include <algorithm>

int** init(int a, int b) {
    int** result = new int* [a];

    for (int i = 0; i < a; i++) {
        result[i] = new int[b];
    }

    return result;
}

void fill(int** mx, int a, int b) {
    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            mx[i][j] = 1000;
        }
    }
}

std::vector<std::pair<int, int>> find(int** matrix, int rows, int cols, int num) {
    std::vector<std::pair<int, int>> result;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == num) {
                result.push_back(std::make_pair(i, j));
            }
        }
    }

    return result;
}

std::vector<std::pair<int, int>> findParallel(int** matrix, int rows, int cols, int num) {
    std::vector<std::pair<int, int>> result;

#pragma omp parallel for collapse(2)
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == num) {
                auto ij = std::make_pair(i, j);
#pragma omp critical
                {
                    result.push_back(ij);
                }
            }
        }
    }

    return result;
}


int main() {
    int a = 5000;
    int b = 5000;
    int n = 999;

    int** mx = init(a, b);
    fill(mx, a, b);

    auto clock = std::chrono::high_resolution_clock();

    auto start = clock.now();
    auto vec_seq = find(mx, a, b, n);
    auto end = clock.now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Seq duration: " << duration.count() << " microseconds" << std::endl;

    start = clock.now();
    auto vec_par = findParallel(mx, a, b, n);
    end = clock.now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Parallel duration: " << duration.count() << " microseconds" << std::endl;

    std::sort(vec_seq.begin(), vec_seq.end());
    std::sort(vec_par.begin(), vec_par.end());

    bool areResultsEqual = vec_seq.size() == vec_par.size();

    for (int i = 0; i < vec_seq.size(); i++) {
        if (!areResultsEqual || vec_seq[i] != vec_par[i]) {
            areResultsEqual = false;
            break;
        }
    }

    if (!areResultsEqual) {
        std::cout << "Results are not equal!" << std::endl;
    }
    else {
        std::cout << "Results are equal" << std::endl;
    }

}
//  2. Визначити для функції обчислювальну складність (T1)
//T1 = a * b

//  3. Обчислити кількість зупинок процесору.
//Кількість зупинок процесору - [2a + b + 2, 2a + b + 2 + a*b]

//  6. Визначити для функції обчислювальну складність (Tp) та обчислити
//  прискорення, ефективність, вартість.
//Tp1 = (a * b / p) + ab - Гірший випадок
//Tp2 = a * b / p - Кращий випадок 

//Sp = T1 / Tp
//Sp1 = T1 / Tp1 = a * b / (a * b / p) + a * b =  p/1+p - Гірший випадок
//Sp2 = T1 / Tp2 = a * b / a * b / p = p - Кращий випадок

//Ep = Sp / p
//Ep1 = Sp1 / p = p / 1 + p/ p - Гірший випадок
//Ep2 = Sp2 / p = p / p = 1 - Кращий випадок

//Cp = p * Tp
//Cp1 = p * Tp1 = p * ((a * b / p) + ab) = ab + abp = ab(1 + p) - Гірший випадок
//Cp2 = p * Tp2 = p * a * b / p = ab - Кращий випадок

//  9. Порівняти теоретичне та експериментальне прискорення.
// Теоретичне: 
//Sp1 = p / 1 + p = 12 / 1 + 12 = 12 / 13 - Гірший випадок
//Sp2 = p = 12 - Кращий випадок

// Практичне: 
//Sp1 = 1852414 / 1867651 = ~0.992 - Гірший випадок
//Sp2 = 16556 / 4659 = ~3.554 - Кращий випадок