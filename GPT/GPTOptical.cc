#include "ortools/linear_solver/linear_solver.h"
#include <iostream>
#include <vector>

using namespace operations_research;

int main() {
    // Create the solver using SCIP backend
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    if (!solver) {
        std::cerr << "SCIP solver is not available." << std::endl;
        return 1;
    }

    // Example parameters (replace with your data)
    const int num_ports = 4; // Example number of ports (assuming square matrix)
    const int num_configurations = 3; // Example number of configurations
    const double c = 10.0; // Example constant data rate
    const double rt = 5.0; // Example reconfiguration time

    // Example demand matrix D (replace with your actual demand matrix)
    std::vector<std::vector<int>> D = {
        {0, 15, 20, 5},
        {5, 0, 10, 15},
        {15, 10, 0, 10},
        {10, 5, 15, 0}
    };

    // Decision variables S_i matrices and t_i variables
    std::vector<std::vector<std::vector<const MPVariable*>>> S(num_configurations);
    std::vector<const MPVariable*> T(num_configurations);

    for (int i = 0; i < num_configurations; ++i) {
        S[i].resize(num_ports);
        for (int j = 0; j < num_ports; ++j) {
            S[i][j].resize(num_ports);
            for (int k = 0; k < num_ports; ++k) {
                // S_i[j][k] is a binary variable
                S[i][j][k] = solver->MakeIntVar(0, 1, "S[" + std::to_string(i) + "][" + std::to_string(j) + "][" + std::to_string(k) + "]");
            }
        }
        // t_i is a non-negative integer variable
        T[i] = solver->MakeIntVar(0, solver->infinity(), "t[" + std::to_string(i) + "]");
    }

    // Constraints: c * (S_1 * t_1 + S_2 * t_2 + ... + S_n * t_n) >= D
    for (int i = 0; i < num_ports; ++i) {
        for (int j = 0; j < num_ports; ++j) {
            LinearExpr constraint_expr;
            for (int k = 0; k < num_configurations; ++k) {
                // Add the product of S[k][i][j] and T[k] as a sum
                constraint_expr += S[k][i][j]->solution_value() * T[k]->solution_value();
            }
            solver->MakeRowConstraint(c * constraint_expr >= D[i][j]);
        }
    }

    // Objective: Minimize total time T = sum(t_i + rt)
    LinearExpr total_time_expr;
    for (int i = 0; i < num_configurations; ++i) {
        total_time_expr += T[i];
    }
    total_time_expr += num_configurations * rt; // Adding reconfiguration time for each configuration
    solver->MutableObjective()->MinimizeLinearExpr(total_time_expr);

    // Solve the problem
    const MPSolver::ResultStatus result_status = solver->Solve();

    // Output results
    if (result_status == MPSolver::OPTIMAL) {
        std::cout << "Optimal solution found:" << std::endl;
        for (int i = 0; i < num_configurations; ++i) {
            std::cout << "Configuration " << i << ":" << std::endl;
            for (int j = 0; j < num_ports; ++j) {
                for (int k = 0; k < num_ports; ++k) {
                    std::cout << S[i][j][k]->solution_value() << " ";
                }
                std::cout << std::endl;
            }
            std::cout << "Duration t_" << i << ": " << T[i]->solution_value() << std::endl;
        }
        std::cout << "Total time: " << solver->Objective().Value() << std::endl;
    } else {
        std::cout << "No optimal solution found." << std::endl;
    }

    return 0;
}
