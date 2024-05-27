#ifndef PARAMETERS_H
#define PARAMETERS_H
#include <array>
#include <complex>
// Structure to hold simulation parameters
struct Parameters {
   public:
    // Constructor
    Parameters(double q_input,
               double shat_input,
               double tau_input,
               double epsilon_n_input,
               double eta_i_input,
               double eta_e_input,
               double b_theta_input,
               double beta_e_input,
               double R_input,
               double vt_input,
               double length_input,
               double theta_input,
               int npoints_input,
               int iteration_step_limit_input);

    // Member variables
    double q;
    double shat;
    double tau;
    double epsilon_n;
    double eta_i;
    double eta_e;
    double b_theta;
    double beta_e;
    double R;
    double vt;
    double alpha;
    double length;
    double theta;
    int npoints;
    int iteration_step_limit;

    // Additional member variables (if needed)
    double omega_s_i;    // Calculated in constructor
    double omega_s_e;    // Calculated in constructor
    double omega_d_bar;  // Calculated in constructor
    virtual void parameterInit();
    virtual double g_integration_f(double eta) const;
    double beta_1(double eta, double eta_p) const;
    double beta_1_e(double eta, double eta_p) const;
    virtual double bi(double eta) const;

    std::complex<double> lambda_f_tau(double eta,
                                      double eta_p,
                                      double tau) const;
    std::complex<double> integration_lambda_tau(
        double eta,
        double eta_p,
        double tau,
        std::array<std::complex<double>, 5> int_arg) const;
    std::complex<double> integration_lambda_d_tau(
        double eta,
        double eta_p,
        double tau,
        std::array<std::complex<double>, 5> int_arg) const;

    std::complex<double> h_f_tau(std::complex<double> omega, double tau) const;

    std::complex<double> kappa_f_tau(unsigned int m,
                                     double eta,
                                     double eta_p,
                                     std::complex<double>) const;

    std::complex<double> kappa_f_tau_e(unsigned int m,
                                       double eta,
                                       double eta_p,
                                       std::complex<double>) const;

   private:
    // No private member functions needed (assuming this is just a data
    // structure)

    std::array<std::complex<double>, 5>
    integration_lambda_arg(double eta, double eta_p, double tau) const;

    /* std::complex<double> lambda_f_tau_term; */
    /* std::complex<double> arg; */
    /* std::complex<double> exp_term; */
    /* std::complex<double> bessel_0; */
    /* std::complex<double> bessel_1; */
};

struct Stellarator : public Parameters {
    Stellarator(double q_input,
                double shat_input,
                double tau_input,
                double epsilon_n_input,
                double eta_i_input,
                double eta_e_input,
                double b_theta_input,
                double beta_e_input,
                double R_input,
                double vt_input,
                double length_input,
                double theta_input,
                int npoints_input,
                int iteration_step_limit_input,
                double eta_k_input,
                int lh_input,
                int mh_input,
                double epsilon_h_t_input,
                double alpha_0_input,
                double r_over_R_input);

    double eta_k;
    int lh;
    int mh;
    double epsilon_h_t;
    double alpha_0;
    double r_over_R;
    double deltap;
    double beta_e_p;
    double rdeltapp;
    double curvature_aver;
    double bi(double eta) const override;
    double g_integration_f(double eta) const override;
    double sigma_f(double eta) const;
    void parameterInit() override;
};

static_assert(std::is_trivially_destructible_v<Parameters>,
              "Paramters should be trivially destructible.");
static_assert(std::is_trivially_destructible_v<Stellarator>,
              "Stellarator should be trivially destructible.");
#endif
