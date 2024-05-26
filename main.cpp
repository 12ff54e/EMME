#include <complex>
#include <fstream>
#include <iostream>
#include <utility>

#include "Grid.h"
#include "JsonParser.h"
#include "Matrix.h"
#include "Parameters.h"
// #include "fenv.h" //this is for check inf or nan
#include "functions.h"
#include "singularity_handler.h"
#include "solver.h"

int main() {
    std::string filename = "input.json";

    auto input = util::json::parse_file(filename);

    std::complex<double> omega_initial_guess(input["initial_guess"][0],
                                             input["initial_guess"][1]);

    Parameters para(input["q"], input["shat"], input["tau"], input["epsilon_n"],
                    input["eta_i"], input["eta_e"], input["b_theta"],
                    input["beta_e"], input["R"], input["vt"], input["length"],
                    input["theta"], input["npoints"],
                    input["iteration_step_limit"]);

    auto length = para.length;
    auto npoints = para.npoints;

    Grid<double> grid_info(length, npoints);

    Matrix<double> coeff_matrix = SingularityHandler(npoints);

    std::ofstream outfile("emme_eigen_vector.csv");
    std::ofstream eigenvalue("emme_eigen_value.csv");

    double tol = 1e-6;

    for (unsigned int i = 0; i <= 40; i++) {
        auto eigen_solver = EigenSolver<Matrix<std::complex<double>>>(
            para, omega_initial_guess, coeff_matrix, grid_info);
        std::cout << eigen_solver.para.q << std::endl;

        for (int j = 0; j <= para.iteration_step_limit; j++) {
            eigen_solver.newtonTraceSecantIteration();
            std::cout << eigen_solver.eigen_value << std::endl;
            if (std::abs(eigen_solver.d_eigen_value) <
                std::abs(tol * eigen_solver.eigen_value)) {
                break;
            }
        }

        std::cout << "Eigenvalue: " << eigen_solver.eigen_value.real() << " "
                  << eigen_solver.eigen_value.imag() << std::endl;
        eigenvalue << eigen_solver.eigen_value.real() << " "
                   << eigen_solver.eigen_value.imag() << std::endl;
        auto null_space = eigen_solver.nullSpace();
        if (!outfile.is_open()) {
            // Handle error
            return 1;
        }
        outfile << null_space;
        flush(eigenvalue);
        flush(outfile);
        para.q += 0.05;
        para.parameterInit();
        omega_initial_guess = eigen_solver.eigen_value;
    }

    outfile.close();

    return 0;
}
