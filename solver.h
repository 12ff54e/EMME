#ifndef SOLVER_H
#define SOLVER_H

// #include <chrono>
#include <complex>
#include <iostream>
#include <vector>

#include "DedicatedThreadPool.h"
#include "Grid.h"
#include "Matrix.h"
#include "Parameters.h"
#include "aligned-allocator.h"

using value_type = std::complex<double>;
using matrix_type = Matrix<value_type, util::AlignedAllocator<value_type>>;

std::pair<value_type, matrix_type> NewtonTraceIteration(value_type lambda,
                                                        double tol);

std::pair<value_type, matrix_type> NewtonTraceIterationSecantMethod(
    value_type lambda,
    const double& tol,
    Parameters& para,
    const Matrix<double>& coeff_matrix,
    const Grid<double>& grid_info,
    const int&);

template <typename T>

Matrix<T> nullSpace(const Matrix<T>& A) {
    // Check if the matrix is square
    if (A.rows() != A.cols()) {
        throw std::invalid_argument("Input matrix must be square.");
    }

    // Perform Singular Value Decomposition (SVD)
    // You'll need an external library like Eigen or LAPACK for SVD
    // Replace this placeholder with your preferred SVD implementation
    // which returns the singular values (S), left singular vectors (U),
    // and right singular vectors (V)
    std::vector<T> S;
    Matrix<T> U;
    Matrix<T> V;
    // ... perform SVD on A and store results in S, U, V

    // Identify null space basis vectors and construct the null space matrix
    int null_space_dim = 0;  // Count the number of null space basis vectors
    for (int i = 0; i < A.cols(); ++i) {
        // Check for singular values close to zero (tolerance approach)
        if (std::abs(S[i]) < std::numeric_limits<T>::epsilon()) {
            null_space_dim++;
        }
    }

    // Create a result matrix to store the null space basis vectors as columns
    Matrix<T> null_space(A.rows(), null_space_dim);
    int col_index = 0;
    for (int i = 0; i < A.cols(); ++i) {
        // Check for singular values close to zero (tolerance approach)
        if (std::abs(S[i]) < std::numeric_limits<T>::epsilon()) {
            // Extract the corresponding right singular vector and store in null
            // space matrix
            null_space.setCol(col_index, U.getCol(i));
            col_index++;
        }
    }

    return null_space;
};  // this is not tested

// Function representing the nonlinear eigenvalue problem (NLEP)
// F(lambda, x) = 0
template <typename Func>
void F(const value_type& tau,
       const value_type& lambda,
       const Func& func,
       const Matrix<double>& coeff_matrix,
       const Grid<double>& grid_info,
       matrix_type& mat) {
    // Implement the NLEP function here
    // This function should return a vector representing the residual (F(lambda,
    // x)) for a given eigenvalue (lambda) and eigenvector (x)
#ifdef EMME_DEBUG
    if (mat.getRows() != grid_info.npoints ||
        mat.getCols() != grid_info.npoints) {
        throw std::runtime_error("Matrix dimension and grid length mismatch.");
    }
#endif
    auto& thread_pool = DedicatedThreadPool<void>::get_instance();
    std::vector<std::future<void>> res;

    for (unsigned int j = 0; j < grid_info.npoints; j++) {
        for (unsigned int i = 0; i < grid_info.npoints; i++) {
            if (i == j) {
                mat(i, j) = (1.0 + 1.0 / tau);
            } else {
                res.push_back(thread_pool.queue_task([&, i, j]() {
                    mat(i, j) =
                        -func(grid_info.grid[i], grid_info.grid[j], lambda) *
                        coeff_matrix(i, j) * grid_info.dx;
                }));
            }
        }
    }
    for (auto& f : res) { f.get(); }
}

template <typename Func>
matrix_type F(const value_type& tau,
              const value_type& lambda,
              const Func& func,
              const Matrix<double>& coeff_matrix,
              const Grid<double>& grid_info) {
    matrix_type quadrature_matrix(grid_info.npoints, grid_info.npoints);
    F(tau, lambda, func, coeff_matrix, grid_info, quadrature_matrix);
    return quadrature_matrix;
}

#endif  // SOLVER_H
