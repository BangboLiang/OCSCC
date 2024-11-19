#pragma once
#include <iostream>
#include <vector>

void printSolution(const std::vector<int>& board, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j == board[i])
                std::cout << "1 ";
            else
                std::cout << "0 ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool isValid(const std::vector<int>& board, int row, int col) {
    if(row == col)
        return false;

    for (int i = 0; i < row; ++i) {
        if (board[i] == col) // Check column 
            return false;
    }
    return true;
}

void solveNQueens(std::vector<int>& board, int row, int n, std::vector<std::vector<int> >& solutions) {
    if (row == n) {
        solutions.push_back(board);
        return;
    }
    for (int col = 0; col < n; ++col) {
        if (isValid(board, row, col)) {
            board[row] = col;
            solveNQueens(board, row + 1, n, solutions);
            board[row] = -1; // callback
        }
    }
}

// int main() {
//     int n;
//     std::cout << "Enter the value of N: ";
//     std::cin >> n;

//     std::vector<int> board(n, -1);
//     std::vector<std::vector<int> > solutions;

//     solveNQueens(board, 0, n, solutions);

//     std::cout << "Number of solutions: " << solutions.size() << std::endl;
//     for (const auto& solution : solutions) {
//         printSolution(solution, n);
//     }

//     return 0;
// }