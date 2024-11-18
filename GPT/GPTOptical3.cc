#include "ortools/linear_solver/linear_solver.h"

namespace operations_research {
void SolveOCSConfiguration() {
    // Create solver instance
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    if (!solver) {
        std::cout << "SCIP solver is unavailable." << std::endl;
        return;
    }

    // Parameters (replace with your actual values)
    int num_ports = 5; // Example value, replace with your actual number of ports
    int num_configurations = 10; // Number of configurations (replace with your specific problem value)
    std::vector<std::vector<int>> traffic_demand = {
        {0, 13, 10, 70, 4},
        {0, 0, 14, 12, 69},
        {65, 0, 0, 12, 14},
        {15, 33, 2, 0, 11},
        {12, 7, 3, 1, 0}
    };
    const double rt = 1.0; // Example value for rt
    const double c = 1.0; // Replace with your scaling constant

    // Variables
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

    // Constraints
    // Traffic demand constraint
    for (int i = 0; i < num_ports; ++i) {
        for (int j = 0; j < num_ports; ++j) {
            LinearExpr constraint_expr;
            for (int k = 0; k < num_configurations; ++k) {
                // Add the product of S[k][i][j] and T[k] as a sum
                constraint_expr += S[k][i][j] * T[k];
            }
            solver->MakeRowConstraint(c * constraint_expr >= traffic_demand[i][j]);
        }
    }

    // OCS configuration constraint
    for (int k = 0; k < num_configurations; ++k) {
        for (int i = 0; i < num_ports; ++i) {
            LinearExpr row_sum;
            for (int j = 0; j < num_ports; ++j) {
                row_sum += S[k][i][j];
            }
            solver->MakeRowConstraint(row_sum == 1);
        }
        for (int j = 0; j < num_ports; ++j) {
            LinearExpr col_sum;
            for (int i = 0; i < num_ports; ++i) {
                col_sum += S[k][i][j];
            }
            solver->MakeRowConstraint(col_sum == 1);
        }
    }

    // Objective function: Minimize total time
    LinearExpr total_time;
    for (int i = 0; i < num_configurations; ++i) {
        total_time += T[i];
    }
    total_time += num_configurations * rt;
    solver->MutableObjective()->MinimizeLinearExpr(total_time);

    // Solve the problem
    const MPSolver::ResultStatus result_status = solver->Solve();
    std::cout << "Result status: ";
    switch (result_status) {
    case MPSolver::OPTIMAL:
        std::cout << "OPTIMAL.\n";
        break;
    case MPSolver::FEASIBLE:
        std::cout << "FEASIBLE.\n";
        break;
    case MPSolver::INFEASIBLE:
        std::cout << "INFEASIBLE.\n";
        break;
    case MPSolver::UNBOUNDED:
        std::cout << "UNBOUNDED.\n";
        break;
    case MPSolver::ABNORMAL:
        std::cout << "ABNORMAL.\n";
        break;
    case MPSolver::MODEL_INVALID:
        std::cout << "MODEL_INVALID.\n";
        break;
    case MPSolver::NOT_SOLVED:
        std::cout << "NOT_SOLVED.\n";
        break;
    default:
        break;
    }

    if (result_status == MPSolver::OPTIMAL || result_status == MPSolver::FEASIBLE) {
        std::cout << "Solution found!" << std::endl;
        for (int i = 0; i < num_configurations; ++i) {
            std::cout << "Configuration " << i << " duration: " << T[i]->solution_value() << std::endl;
            std::cout << "Configuration matrix:" << std::endl;
            for (int j = 0; j < num_ports; ++j) {
                for (int k = 0; k < num_ports; ++k) {
                    std::cout << S[i][j][k]->solution_value() << " ";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "Total Time: " << solver->Objective().Value() << std::endl;
    } else {
        std::cout << "No feasible solution found." << std::endl;
    }
}
}  // namespace operations_research

int main() {
    operations_research::SolveOCSConfiguration();
    return 0;
}
