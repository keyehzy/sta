# Geometric Algebra Multivector

A simple C++ implementation of geometric algebra multivectors demonstrating the addition and geometric product operations. This project provides a basic framework for working with blades and multivectors, ideal for educational purposes and experimentation with geometric algebra.

## Features

- **Multivector Operations:** Supports addition and the geometric product.
- **Basis Vector Creation:** Easily create basis vectors for geometric algebra.
- **Operator Overloading:** Intuitive arithmetic operations with overloaded operators.

## Requirements

- A C++ compiler.
- GNU Make.

## Building

To build the project, simply run:

```bash
make
```

This command produces an executable named `multivector`.

## Running

After building the project, execute the following command to see the demonstration:

```bash
./multivector

Basis Vectors:
e1: 1 * e(1)

e2: 1 * e(2)

e3: 1 * e(4)


Geometric Products of Basis Vectors:
e1 * e1 = 1 * e(0)

e1 * e2 = -1 * e(3)

e1 * e3 = -1 * e(5)

e2 * e1 = 1 * e(3)

e2 * e2 = 1 * e(0)

e2 * e3 = -1 * e(6)

e3 * e1 = 1 * e(5)

e3 * e2 = 1 * e(6)

e3 * e3 = 1 * e(0)


Pseudoscalar (e1 * e2 * e3):
-1 * e(7)
```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
