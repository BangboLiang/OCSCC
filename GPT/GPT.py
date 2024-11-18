from ortools.linear_solver import pywraplp

def solve_ocs_optimization(D, c, rt):
    # Create the solver
    solver = pywraplp.Solver.CreateSolver('SCIP')
    if not solver:
        return None

    n = len(D)  # Number of configurations
    m = len(D[0])  # Number of ports (assuming square matrix for simplicity)

    # Variables for configuration matrices and their durations
    S = [[[solver.IntVar(0, 1, f'S_{i}_{j}_{k}') for k in range(m)] for j in range(m)] for i in range(n)]
    t = [solver.NumVar(0, solver.infinity(), f't_{i}') for i in range(n)]

    # Objective function: Minimize total time
    total_time = solver.Sum(t[i] + rt for i in range(n))
    solver.Minimize(total_time)

    # Constraints for demand satisfaction
    for i in range(m):
        for j in range(m):
            demand_constraint = solver.Sum(S[k][i][j] * t[k] for k in range(n)) * c
            solver.Add(demand_constraint >= D[i][j])

    # Constraints for each configuration matrix being a permutation matrix
    for k in range(n):
        for i in range(m):
            solver.Add(solver.Sum(S[k][i][j] for j in range(m)) == 1)
        for j in range(m):
            solver.Add(solver.Sum(S[k][i][j] for i in range(m)) == 1)

    # Solve the problem
    status = solver.Solve()

    if status == pywraplp.Solver.OPTIMAL:
        solution_S = [[[S[i][j][k].solution_value() for k in range(m)] for j in range(m)] for i in range(n)]
        solution_t = [t[i].solution_value() for i in range(n)]
        return solution_S, solution_t, solver.Objective().Value()
    else:
        print("No optimal solution found.")
        return None

# Example usage
D = [[...]]  # Replace with actual demand matrix
c = 1  # Example scaling factor
rt = 2  # Example reconfiguration time
result = solve_ocs_optimization(D, c, rt)
print(result)
