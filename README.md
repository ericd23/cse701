# bigint

`bigint` class is a arbitrary-precision integer type implemented using C++:

- Constructor taking 64-bit signed integer or strings.
- Arithmetic: addition, subtraction and multiplication.
- Increment and decrement (postfix and prefix).
- Comparison (`<`, `<=`, `==`, `!=`, `>`, `>=`).
- Printing to output string stream.

## Internal

Internally, it stores the number as a sequence of 8-bit digits (0-9) in reversed order. An additional bool is used to indicate the sign.

It does basic arithmetic operations by immiating the way we do by hand, that is:
- adding digits one by one and carrying over when necessary.
- subtracting digits one by one and borrowing when necessary.
- multiplying digits one by one and adding the results.

Other operations are implemented using these basic operations.

Normalizing is done after each operation to remove leading zeros.

## Usage

Constructors:
```cpp
bigint a("99999999999999999999");
std::cout << a << "\n"; // Prints: 99999999999999999999
```

Addition:
```cpp
bigint a("999999999999999999999");
bigint b(1);
bigint c = a + b;
std::cout << c << "\n"; // Prints: 1000000000000000000000
```

Subtraction:
```cpp
bigint a(10);
bigint b(20);
bigint c = a - b;
std::cout << c << "\n"; // Prints: -10
```

Multiplication:
```cpp
bigint a(1111111111);
bigint b(2222222222);
bigint c = m * n;
std::cout << c << "\n"; // Prints: 2469135801975308642
```

Increment and decrement:
```cpp
bigint val(999);
++val;
std::cout << val << "\n"; // Prints: 1000

val--;
std::cout << val << "\n"; // Prints: 999
```

Comparison:
```cpp
bigint a(10);
bigint b(20);
std::cout << (a < b) << "\n"; // Prints: 1
```
