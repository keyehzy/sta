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

    friend std::ostream& operator<<(std::ostream& os, const Blade &b) {
        os << b.coefficient << " * e(" << b.mask << ")";
        return os;
    }
};

template <size_t Dimension>
struct EuclideanSignature {
    static constexpr size_t max_dimension() {
        return Dimension;
    }

    static constexpr uint32_t value(std::size_t) {
        return 1;
    }
};

struct MinkowskiSignature {
    static constexpr int32_t signature[4] = {1, 0, 0, 0};

    static constexpr size_t max_dimension() {
        return 4;
    }

    static constexpr int32_t value(size_t i) {
        assert(i < max_dimension() && "Index outside of signature bounds");
        return signature[i];
    }
};

template <class Signature>
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
        assert(i < Signature::max_dimension() && "Basis vector index exceed maximum value");
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
                int32_t s = sign(a.mask, b.mask);
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
        for (size_t i = 0; i < v.m_blades.size(); i++) {
            os << v.m_blades[i] << (i < v.m_blades.size() - 1 ? "\n" : "");
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

    static constexpr int32_t sign(uint64_t a, uint64_t b) {
        uint64_t parity = blade_parity(a, b);

        // Extra code necessary for handling different metrics
        uint64_t repeated = a & b;
        while (repeated) {
            uint64_t i = __builtin_ctzll(repeated);
            parity ^= Signature::value(i);
            repeated &= (repeated - 1);
        }
        return 2 * parity - 1;
    }

    static constexpr uint64_t blade_parity(uint64_t a, uint64_t b) {
        uint64_t parity = 0;
        while (b) {
            uint64_t lowest_set_bit = __builtin_ctzll(b);
            uint64_t count_bits_below = __builtin_popcountll(a & ((1ULL << lowest_set_bit) - 1));
            parity ^= count_bits_below & 1;
            b &= b - 1;
        }
        return parity & 1;
    }

private:
    std::vector<Blade> m_blades;
};

using CliffordMultivector = Multivector<EuclideanSignature<64>>;
using EuclideanMultivector = Multivector<EuclideanSignature<4>>;
using SpacetimeMultivector = Multivector<MinkowskiSignature>;

int main() {
    std::vector<SpacetimeMultivector> basis = {
        SpacetimeMultivector::basis_vector(0),
        SpacetimeMultivector::basis_vector(1),
        SpacetimeMultivector::basis_vector(2),
        SpacetimeMultivector::basis_vector(3),
    };

    std::cout << "Basis Vectors:" << std::endl;
    for (size_t i = 0; i < basis.size(); ++i) {
        std::cout << "e" << i + 1 << ": " << basis[i] << std::endl;
    }

    std::cout << "\nBivectors:" << std::endl;
    for (size_t i = 0; i < basis.size(); ++i) {
        for (size_t j = i; j < basis.size(); ++j) {
            std::cout << "e" << i + 1 << " * e" << j + 1 
                << " = " << (basis[i] * basis[j]) << std::endl;
        }
    }

    std::cout << "\nTrivectors:" << std::endl;
    for (size_t i = 0; i < basis.size(); ++i) {
        for (size_t j = i; j < basis.size(); ++j) {
            for (size_t k = j; k < basis.size(); ++k) {
                std::cout << "e" << i + 1 << " * e" << j + 1 << " * e" << k + 1
                    << " = " << (basis[i] * basis[j] * basis[k]) << std::endl;
            }
        }
    }

    std::cout << "\nPseudoscalar (e1 * e2 * e3 * e4):" << std::endl;
    std::cout << basis[0] * basis[1] * basis[2] * basis[3] << std::endl;

    return 0;
}
