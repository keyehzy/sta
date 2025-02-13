/**
 * Geometric Algebra Multivector Implementation
 *
 * This program demonstrates a simple implementation of geometric algebra
 * multivectors. The Multivector class supports addition and the geometric product.
 *
 * For more details on geometric algebra, see:
 * https://en.wikipedia.org/wiki/Geometric_algebra#Blades,_grades,_and_basis
 */

#include <vector>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <cassert>

struct Blade {
    float coefficient;
    uint64_t mask;
};

class Multivector {
public:
    static Multivector create(const std::initializer_list<Blade>& blades) {
        Multivector v;
        for (const auto &b : blades) {
            v.add_blade(b.coefficient, b.mask);
        }
        return v;
    }

    static Multivector basis_vector(uint64_t i) {
        assert(i < max_dimension && "Basis vector index exceed maximum value");
        Multivector v;
        v.add_blade(1.0f, 1ULL << i);
        return v;
    }

    Multivector operator+(const Multivector &other) const {
        Multivector result = *this;
        for (const auto &b : other.m_blades) {
            result.add_blade(b.coefficient, b.mask);
        }
        return result;
    }

    Multivector operator-(const Multivector &other) const {
        Multivector result = *this;
        for (const auto &b : other.m_blades) {
            result.add_blade(-b.coefficient, b.mask);
        }
        return result;
    }

    Multivector operator*(float scalar) const {
        Multivector result;
        for (const auto &b : m_blades) {
            result.add_blade(scalar * b.coefficient, b.mask);
        }
        return result;
    }

    Multivector operator*(const Multivector &other) const {
        Multivector result;
        for (const auto &a : m_blades) {
            for (const auto &b : other.m_blades) {
                uint64_t new_mask = a.mask ^ b.mask;
                int32_t s = blade_sign(a.mask, b.mask);
                float new_coeff = a.coefficient * b.coefficient * s;
                result.add_blade(new_coeff, new_mask);
            }
        }
        return result;
    }

    Multivector reverse() const {
        Multivector result;
        for (const auto &b : m_blades) {
            uint64_t grade = __builtin_popcountll(b.mask);
            uint64_t parity = (grade * (grade - 1) / 2) % 2;
            int32_t sign = 1 - 2 * parity;
            result.add_blade(b.coefficient * sign, b.mask);
        }
        return result;
    }

    static Multivector commutator(const Multivector &A, const Multivector &B) {
        return A * B - B * A;
    }

    static Multivector anticommutator(const Multivector &A, const Multivector &B) {
        return A * B + B * A;
    }

    friend Multivector operator*(float scalar, const Multivector& v) {
        return v * scalar;
    }

    friend std::ostream& operator<<(std::ostream& os, const Multivector &v) {
        if (!v.m_blades.empty()) {
            for (const auto &b : v.m_blades) {
                os << b.coefficient << " * e(" << b.mask << ")\n";
            }
        }
        return os;
    }

private:
    Multivector() = default;

    void add_blade(float coeff, uint64_t mask) {
        if (coeff == 0.0f) {
            return;
        }
        for (auto &b : m_blades) {
            if (b.mask == mask) {
                b.coefficient += coeff;
                return;
            }
        }
        m_blades.push_back({coeff, mask});
    }

    static constexpr int32_t blade_sign(uint64_t a, uint64_t b) noexcept {
        uint64_t parity = 0;
        while (b) {
            uint64_t lowest_set_bit = __builtin_ctzll(b);
            uint64_t count_bits_below = __builtin_popcountll(a & ((1ULL << lowest_set_bit) - 1));
            parity ^= count_bits_below & 1;
            b &= b - 1;
        }
        return 1 - 2 * (parity & 1);
    }

    std::vector<Blade> m_blades;
    static constexpr uint32_t max_dimension = 64;
};

int main() {
    std::vector<Multivector> basis = {
        Multivector::basis_vector(0),
        Multivector::basis_vector(1),
        Multivector::basis_vector(2),
    };

    std::cout << "Basis Vectors:" << std::endl;
    for (size_t i = 0; i < basis.size(); ++i) {
        std::cout << "e" << i + 1 << ": " << basis[i] << std::endl;
    }

    std::cout << "\nGeometric Products of Basis Vectors:" << std::endl;
    for (size_t i = 0; i < basis.size(); ++i) {
        for (size_t j = 0; j < basis.size(); ++j) {
            std::cout << "e" << i + 1 << " * e" << j + 1 
                << " = " << (basis[i] * basis[j]) << std::endl;
        }
    }

    std::cout << "\nPseudoscalar (e1 * e2 * e3):" << std::endl;
    std::cout << basis[0] * basis[1] * basis[2] << std::endl;

    return 0;
}
