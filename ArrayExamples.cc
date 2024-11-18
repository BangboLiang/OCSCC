/*
Solve this problem:
Maximize 7x1 + 8x2 + 2x3 + 9x4 + 6x5 subject to the following constraints:

5 x1 + 7 x2 + 9 x3 + 2 x4 + 1 x5 ≤ 250
18 x1 + 4 x2 - 9 x3 + 10 x4 + 12 x5 ≤ 285
4 x1 + 7 x2 + 3 x3 + 8 x4 + 5 x5 ≤ 211
5 x1 + 13 x2 + 16 x3 + 3 x4 - 7 x5 ≤ 315

*/

#include <memory>
#include <vector>

#include "ortools/linear_solver/linear_solver.h"

namespace operations_research {
struct DataModel {
  const std::vector<std::vector<double>> constraint_coeffs{
      {5, 7, 9, 2, 1},
      {18, 4, -9, 10, 12},
      {4, 7, 3, 8, 5},
      {5, 13, 16, 3, -7},
  };
  const std::vector<double> bounds{250, 285, 211, 315};
  const std::vector<double> obj_coeffs{7, 8, 2, 9, 6};
  const int num_vars = 5;
  const int num_constraints = 4;
};

void MipVarArray() {
  DataModel data;

  // Create the mip solver with the SCIP backend.
  std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
  if (!solver) {
    LOG(WARNING) << "SCIP solver unavailable.";
    return;
  }

  const double infinity = solver->infinity();
  // x[j] is an array of non-negative, integer variables.
  std::vector<const MPVariable*> x(data.num_vars);
  for (int j = 0; j < data.num_vars; ++j) {
    x[j] = solver->MakeIntVar(0.0, infinity, "");
  }
  LOG(INFO) << "Number of variables = " << solver->NumVariables();

  // Create the constraints.
  for (int i = 0; i < data.num_constraints; ++i) {
    MPConstraint* constraint = solver->MakeRowConstraint(0, data.bounds[i], "");
    for (int j = 0; j < data.num_vars; ++j) {
      constraint->SetCoefficient(x[j], data.constraint_coeffs[i][j]);
    }
  }
  LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

  // Create the objective function.
  MPObjective* const objective = solver->MutableObjective();
  for (int j = 0; j < data.num_vars; ++j) {
    objective->SetCoefficient(x[j], data.obj_coeffs[j]);
  }
  objective->SetMaximization();

  const MPSolver::ResultStatus result_status = solver->Solve();

  // Check that the problem has an optimal solution.
  if (result_status != MPSolver::OPTIMAL) {
    LOG(FATAL) << "The problem does not have an optimal solution.";
  }
  LOG(INFO) << "Solution:";
  LOG(INFO) << "Optimal objective value = " << objective->Value();

  for (int j = 0; j < data.num_vars; ++j) {
    LOG(INFO) << "x[" << j << "] = " << x[j]->solution_value();
  }
}
}  // namespace operations_research

int main(int argc, char** argv) {
  operations_research::MipVarArray();
  return EXIT_SUCCESS;
}