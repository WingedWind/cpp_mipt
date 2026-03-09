#include <iostream>
#include <string>
#include <random>
#include <vector>

int metric(const std::string& a, const std::string& b)
{
    int m = 0;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i] != b[i])
            ++m;
    return m;
}

char random_letter(std::default_random_engine& gen)
{
    static std::uniform_int_distribution<int> dist('a', 'z');
    return static_cast<char>(dist(gen));
}

std::string random_string(size_t n, std::default_random_engine& gen)
{
    std::string s(n, 'a');
    for (auto& c : s)
        c = random_letter(gen);
    return s;
}

std::string mutate(
    const std::string& parent,
    std::default_random_engine& gen,
    std::uniform_real_distribution<double>& prob)
{
    std::string child = parent;

    for (auto& c : child)
    {
        if (prob(gen) < 0.05)
            c = random_letter(gen);
    }

    return child;
}

int main()
{
    const std::string target = "methinksitislikeaweasel";
    const size_t length = target.size();
    const int population = 100;

    std::random_device rd;
    std::default_random_engine gen(rd());

    std::uniform_real_distribution<double> prob(0.0, 1.0);

    std::string parent = random_string(length, gen);

    int iteration = 0;

    while (true)
    {
        ++iteration;

        std::vector<std::string> children(population);

        for (auto& c : children)
            c = mutate(parent, gen, prob);

        int best_metric = length;
        std::string best;

        for (auto& c : children)
        {
            int m = metric(c, target);

            if (m < best_metric)
            {
                best_metric = m;
                best = c;
            }

            if (m == 0)
            {
                std::cout << "Iteration " << iteration
                          << " : " << c << std::endl;
                return 0;
            }
        }

        parent = best;

        std::cout << "Iteration " << iteration
                  << " : " << parent
                  << " (metric = " << best_metric << ")"
                  << std::endl;
    }
}
