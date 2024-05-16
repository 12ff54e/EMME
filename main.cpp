#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include "Grid.h"
#include "Matrix.h"
#include "Parameters.h"
// #include "function.h"
#include "singularity_handler.h"
#include "solver.h"

int main() {
    std::string filename = "emme.in";

    std::ifstream input_file(filename);

    double q_input;
    double shat_input;
    double tau_input;
    double epsilon_n_input;
    double eta_i_input;
    double b_theta_input;
    double R_input;
    double vt_input;
    double length_input;
    double theta_input;
    int npoints_input;
    int iteration_step_limit_input;

    input_file >> q_input >> shat_input >> tau_input >> epsilon_n_input >>
        eta_i_input >> b_theta_input >> R_input >> vt_input >> length_input >>
        theta_input >> npoints_input >> iteration_step_limit_input;

    input_file.close();

    // const auto [q; shat, tau, epsilon_n, eta_i, b_theta, R, vt,
    // length, theta,
    //             npoints, iteration_step_limit] =
    //             readInputFromFile(filename);

    // std::tie(omega_s_i, omega_d_bar) = initiallize(vt, tau);

    std::complex<double> omega_initial_guess(1.0, 1.0);

    double length = 10.0;
    int npoints = 32;
    Grid grid_info(length, npoints);
    Parameters para(q_input, shat_input, tau_input, epsilon_n_input,
                    eta_i_input, b_theta_input, R_input, vt_input, length_input,
                    theta_input, npoints_input, iteration_step_limit_input);

    // Matrix iter_matrix =
    //     matrix_assembler(tau, omega_iter, kappa_f, coeff_matrix,
    //     grid_info);

    // Matrix coeff_matrix =
    //     singularity_handler(grid_info, gauss_order, interpolation_order);

    Matrix coeff_matrix = SingularityHandler(npoints);

    // IterateSolver iter_solver(tau, omega_initial, matrix_assembler,
    //                           coeff_matrix, grid_info, iteration_step_limit);

    // iter_solver.run();

    // Vector eigen_vector = eigen_vector_solver(iter_solver.matrix);

    // output(filename);

    std::complex<double> lambda0 = 2.4;

    // Set the tolerance for convergence
    double tol = 1e-6;

    std::pair<std::complex<double>, Matrix<std::complex<double>>> result =
        NewtonTraceIterationSecantMethod(lambda0, tol);

    std::cout << "Eigenvalue: " << result.first << std::endl;

    return 0;
}