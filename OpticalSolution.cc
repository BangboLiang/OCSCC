#include <iostream>
#include "NQueen.h"
#include <vector>
#include <memory>
#include "ortools/linear_solver/linear_solver.h"

#define USE_INPUT_BOUND 1

// const std::vector<std::vector<double>> constraint_coeffs{
//       {0, 7, 9, 2},
//       {18, 0, 9, 10},
//       {4, 7, 0, 8},
//       {5, 13, 16, 0},
//   };

//replace this to Traffic demand matrix.
const std::vector<std::vector<double> > bounds{
    {0, 7, 9, 2, 1, 6, 15, 17, 15, 20}, 
    {18, 0, 9, 10, 12, 8, 15, 12, 20, 15}, 
    {4, 7, 0, 8, 5, 12, 14, 16, 18, 20}, 
    {5, 13, 16, 0, 7, 7, 15, 6, 4, 2},
    {7, 12, 4, 3, 0, 9, 4, 7, 12, 10},
    {9, 17, 12, 8, 6, 0, 12, 11, 10, 9},
    {18, 10, 9, 10, 12, 8, 0, 10, 11, 6}, 
    {4, 7, 10, 8, 5, 12, 12, 0, 10, 15 }, 
    {5, 13, 16, 10, 7, 7, 15, 10, 0, 10},
    {7, 12, 4, 3, 10, 9, 10, 6, 14, 0},
};

std::vector<std::vector<double> > input_bounds;

int num_vars;           //How many variables in the ILP problem.
int num_constraints;

int main(int argc, char const *argv[])
{
    int width;
    std::cout << "Enter the matrix width:";
    std::cin >> width;
    //num_vars = width;
    std::vector<int> NQboard(width, -1);
    input_bounds.resize(width, std::vector<double>(width));
    if (USE_INPUT_BOUND)
    {
        std::cout << "Input the traffic demand matrix:(" << width << '*' << width << "):\n";
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < width; j++)
            {
                std::cin>> input_bounds[i][j];
            }
        }
    }
    
    std::vector<std::vector<int> > NQsolutions;
    solveNQueens(NQboard, 0, width, NQsolutions);
    std::vector<std::vector<int> >::iterator it;
    std::vector<std::vector<std::vector<double> > > constraint_coeffs;
    std::cout << "N Queen Solution num: " << NQsolutions.size() << '\n';
    num_constraints = width*width;

    num_vars = NQsolutions.size();
    std::vector<const operations_research::MPVariable*> t(num_vars);

    // for (it = NQsolutions.begin(); it != NQsolutions.end(); it++)
    // {
    //     printSolution(*it, width);
    // }
    int index = 0;
    constraint_coeffs.resize(NQsolutions.size(), std::vector<std::vector<double>>(width, std::vector<double>(width, 0)));
    for (it = NQsolutions.begin(); it != NQsolutions.end(); it++)
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (j == (*it)[i])
                {
                    constraint_coeffs[index][i][j] = 1 ;
                }
                else
                {
                    constraint_coeffs[index][i][j] = 0 ;
                }
            }
        }
        index++;
    }
    //std::cout << "constraint_coeffs.size() is " << constraint_coeffs.size() << '\n';
    // std::cout << "constraint_coeffs printing.\n";
    //Debug: show all constraint_coeffs:
    // for (int i = 0; i < constraint_coeffs.size(); i++)
    // {
    //     std::cout << "i: " << i << '\n';
    //     for (int j = 0; j < width; j++)
    //     {
    //         for (int k = 0; k < width; k++)
    //         {
    //             std::cout << constraint_coeffs[i][j][k] << ' ';
    //         }
    //         std::cout << '\n';
    //     }
    // }
    //std::cout << "constraint_coeffs print finished.\n";
    std::unique_ptr<operations_research::MPSolver> solver(operations_research::MPSolver::CreateSolver("SCIP"));
    if (!solver) {
        LOG(WARNING) << "SCIP solver unavailable.";
        return -1;
    }

    const double infinity = solver->infinity();
    // x[j] is an array of non-negative, integer variables.
    for (int j = 0; j < num_vars; ++j) {
        t[j] = solver->MakeIntVar(0.0, infinity, "");
    }
    
    LOG(INFO) << "Number of variables = " << solver->NumVariables();
    // Create the constraints.
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < width; j++)
        {
            operations_research::MPConstraint* constraint = solver->MakeRowConstraint(input_bounds[i][j], infinity, "");
            for (int k = 0; k < num_vars; k++)
            {
                constraint->SetCoefficient(t[k],constraint_coeffs[k][i][j]);
            }
        }
    }
    LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

    // Create the objective function.
    operations_research::MPObjective* const objective = solver->MutableObjective();
    for (int i = 0; i < num_vars; i++)
    {
        objective->SetCoefficient(t[i], 1.0);
    }
    objective->SetMinimization();
    const operations_research::MPSolver::ResultStatus result_status = solver->Solve();
    if (result_status != operations_research::MPSolver::OPTIMAL) {
        LOG(FATAL) << "The problem does not have an optimal solution.";
    }
    LOG(INFO) << "Solution:";
    LOG(INFO) << "Optimal objective value = " << objective->Value();
    // for (int j = 0; j < num_vars; ++j) {
    //     LOG(INFO) << "x[" << j << "] = " << t[j]->solution_value();
    // }

    int configure_nums = 0;
    for (int i = 0; i < num_vars; i++)
    {
        if (t[i]->solution_value())
        {
            std::cout << "Configure " << ++configure_nums << ":\n";
            for (int k = 0; k < width; k++)
            {
                for (int kk = 0; kk < width; kk++)
                {
                    std::cout << constraint_coeffs[i][k][kk] << ' ';
                }
                std::cout << '\n';
            }
            std::cout << "Duration time: " << t[i]->solution_value() << '\n';
        }
        
    }
    
    std::vector<std::vector<double> > total_bytes;
    total_bytes.resize(width, std::vector<double>(width));
    for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < width; j++)
            {
                total_bytes[i][j] = 0;
            }
            
        }
    for (int index = 0; index < num_vars; index++)
    {
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < width; j++)
            {
                total_bytes[i][j] += t[index]->solution_value() * constraint_coeffs[index][i][j];
            }
        }
    }

    std::cout << "The total Byte transported:\n";
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < width; j++)
        {
            std::cout << total_bytes[i][j] << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "\"Over Subscribed\" Bytes:\n";
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < width; j++)
        {
            std::cout << total_bytes[i][j] - input_bounds[i][j] << ' ';
        }
        std::cout << '\n';
    }
    return 0;
}
