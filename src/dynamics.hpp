#include "ball.hpp"
#include <vector>

class Dynamics {
public:
    static constexpr double rho = 1.225; // Air Density (kg/m^3)
    static constexpr double nu = 0.0000181; // Air Viscosity
    static constexpr double nu_k = 0.0000148; // Air Kinematic Viscosity

    static constexpr double nu_g = 0.00012; // Grass Viscosity (estimate somewhere between air and water)

    // define some common vectors
    static constexpr Vector3 x_axis = {1.0f, 0.0f, 0.0f};
    static constexpr Vector3 y_axis = {0.0f, 1.0f, 0.0f};
    static constexpr Vector3 z_axis = {0.0f, 0.0f, 1.0f};

    static void rk4(Ball *ball, double delta_time);
    static void simple_integral(Ball *ball, double delta_time);
};
