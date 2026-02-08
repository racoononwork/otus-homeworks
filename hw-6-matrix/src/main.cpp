#include "include/infinity_matrix.hpp"


int main() {
    Matrix<int, -1> matrix;
    for (int i = 0; i < 10; ++i) {
        matrix[i][i] = i;               
        matrix[i][9 - i] = 9 - i;       
    }

    for (int i = 1; i <= 8; ++i) {
        for (int j = 1; j <= 8; ++j) {
            std::cout << matrix[i][j] << (j == 8 ? "" : " ");
        }
        std::cout << std::endl;
    }
    matrix[100][100] = 222;
    std::cout << "Occupied: " << matrix.size() << std::endl;
    for (auto c : matrix) {
        size_t x, y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << "[" << x << "][" << y << "] = " << v << std::endl;
    }

    return 0;
}
