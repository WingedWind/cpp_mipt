#include <array>
#include <cmath>

consteval double compute_pi(double epsilon) {
    double sum = 0.0;
    double term = 1.0;
    int n = 0;

    while (std::abs(term) >= epsilon) {
        term = ((n % 2 == 0) ? 1.0 : -1.0) / (2 * n + 1);
        sum += term;
        ++n;
    }

    return 4.0 * sum;
}

consteval double compute_e(double epsilon) {
    double sum = 1.0;
    double term = 1.0;
    int n = 1;

    while (term >= epsilon) {
        term /= n;
        sum += term;
        ++n;
    }

    return sum;
}

constexpr std::array<double, 2> epsilons{
    1e-3,
    1e-4,
};

static_assert(compute_pi(epsilons[0]) > 3.13 &&
              compute_pi(epsilons[0]) < 3.15);

static_assert(compute_pi(epsilons[1]) > 3.141 &&
              compute_pi(epsilons[1]) < 3.142);

static_assert(compute_e(epsilons[0]) > 2.71 &&
              compute_e(epsilons[0]) < 2.73);

static_assert(compute_e(epsilons[1]) > 2.718 &&
              compute_e(epsilons[1]) < 2.719);

int main() {
    return 0;
}

