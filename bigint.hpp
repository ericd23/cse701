#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

/**
 * @class bigint
 * @brief large integer value with arbitrary precision.
 */
class bigint {
public:
  /**
   * @brief Default constructor. Creates a bigint with zero value.
   */
  bigint() : digits(1, 0), ne(false){};

  /**
   * @brief Constructs a bigint from a signed 64-bit integer.
   * @param num The integer value to be used.
   */
  bigint(int64_t num) {
    if (num == 0) {
      digits = std::vector<uint8_t>(1, 0);
      ne = false;
      return;
    } else if (num < 0) {
      ne = true;
      num = -num;
    } else {
      ne = false;
    }

    while (num > 0) {
      digits.push_back(static_cast<uint8_t>(num % 10));
      num /= 10;
    }
  };

  /**
   * @brief Constructs a bigint from a string.
   * @param num A string containing the number, starting with '-' if negative.
   * @throw std::invalid_argument if string is empty or contains invalid
   * characters.
   */
  bigint(const std::string &num) {
    if (num.empty())
      throw std::invalid_argument("Empty string");

    size_t i = 0;
    ne = false;
    if (num[0] == '-') {
      ne = true;
      i = 1;
    }

    for (; i < num.size(); i++) {
      if (!std::isdigit(num[i]))
        throw std::invalid_argument("Invalid character");
      digits.push_back(static_cast<uint8_t>(num[i] - '0'));
    }

    if (digits.empty())
      throw std::invalid_argument("String does not contain any digits");

    std::reverse(digits.begin(), digits.end());
    normalize();
  };

  /**
   * @brief Addition operator.
   * @param rhs The right-hand side bigint of the addition.
   * @return The sum of the two bigints.
   */
  bigint operator+(const bigint &rhs) const {
    if (ne == rhs.ne)
      return add(rhs);
    return sub(rhs);
  };

  /**
   * @brief Addition assignment operator.
   * @param rhs The right-hand side bigint of the addition.
   * @return The sum of the two bigints.
   */
  bigint operator+=(const bigint &rhs) {
    *this = *this + rhs;
    return *this;
  };

  /**
   * @brief Subtraction operator.
   * @param rhs The right-hand side bigint of the subtraction.
   * @return The difference of the two bigints.
   */
  bigint operator-(const bigint &rhs) const {
    if (ne == rhs.ne)
      return sub(rhs);
    return add(rhs);
  };

  /**
   * @brief Subtraction assignment operator.
   * @param rhs The right-hand side bigint of the subtraction.
   * @return The difference of the two bigints.
   */
  bigint operator-=(const bigint &rhs) {
    *this = *this - rhs;
    return *this;
  };

  /**
   * @brief Multiplication operator.
   * @param rhs The right-hand side bigint of the multiplication.
   * @return The product of the two bigints.
   */
  bigint operator*(const bigint &rhs) const {
    bigint result;
    result.ne = (ne != rhs.ne);
    result.digits.resize(digits.size() + rhs.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); i++) {
      uint8_t c = 0;
      for (size_t j = 0; j < rhs.digits.size() || c > 0; j++) {
        uint64_t product =
            result.digits[i + j] +
            digits[i] * (j < rhs.digits.size() ? rhs.digits[j] : 0) + c;
        result.digits[i + j] = static_cast<uint8_t>(product % 10);
        c = static_cast<uint8_t>(product / 10);
      }
    }

    result.normalize();
    return result;
  };

  /**
   * @brief Multiplication assignment operator.
   * @param rhs The right-hand side bigint of the multiplication.
   * @return The product of the two bigints.
   */
  bigint operator*=(const bigint &rhs) {
    *this = *this * rhs;
    return *this;
  };

  /**
   * @brief Negation operator.
   * @return The negation of the bigint.
   */
  bigint operator-() const {
    bigint result = *this;
    result.ne = !ne;
    result.normalize();
    return result;
  };

  /**
   * @brief Equality operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if the two bigints are equal, false otherwise.
   */
  bool operator==(const bigint &rhs) const {
    return ne == rhs.ne && digits == rhs.digits;
  };

  /**
   * @brief Inequality operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if the two bigints are not equal, false otherwise.
   */
  bool operator!=(const bigint &rhs) const { return !(*this == rhs); };

  /**
   * @brief Less than operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if this bigint is less than the right-hand side, false
   * otherwise.
   */
  bool operator<(const bigint &rhs) const {
    // Sign
    if (ne != rhs.ne)
      return ne; // ne != po => true => <, po != ne => false => >
    if (!ne)
      return abs_less(rhs);
    return rhs.abs_less(*this);
  };

  /**
   * @brief Less than or equal operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if this bigint is less than or equal to the right-hand side,
   * false otherwise.
   */
  bool operator<=(const bigint &rhs) const {
    return *this < rhs || *this == rhs;
  };
  /**
   * @brief Greater than operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if this bigint is greater than the right-hand side, false
   * otherwise.
   */
  bool operator>(const bigint &rhs) const { return !(*this <= rhs); };
  /**
   * @brief Greater than or equal operator.
   * @param rhs The right-hand side bigint of the comparison.
   * @return True if this bigint is greater than or equal to the right-hand
   * side, false otherwise.
   */
  bool operator>=(const bigint &rhs) const { return !(*this < rhs); };

  /**
   * @brief Insertion operator.
   * @param stream The stream to write to.
   * @param num The bigint to write.
   * @return The stream.
   */
  friend std::ostream &operator<<(std::ostream &stream, const bigint &num) {
    if (num.ne)
      stream << '-';
    for (size_t i = num.digits.size(); i > 0; i--) {
      stream << static_cast<char>(num.digits[i - 1] + '0');
    }
    return stream;
  };

  /**
   * @brief Prefix increment operator.
   * @return The incremented bigint.
   */
  bigint &operator++() {
    *this = *this + bigint(1);
    return *this;
  };
  /**
   * @brief Postfix increment operator.
   * @return The original bigint.
   */
  bigint operator++(int) {
    bigint tmp = *this;
    *this = *this + bigint(1);
    return tmp;
  };
  /**
   * @brief Prefix decrement operator.
   * @return The decremented bigint.
   */
  bigint &operator--() {
    *this = *this - bigint(1);
    return *this;
  };
  /**
   * @brief Postfix decrement operator.
   * @return The original bigint.
   */
  bigint operator--(int) {
    bigint tmp = *this;
    *this = *this - bigint(1);
    return tmp;
  };

private:
  /**
   * @brief Internal storage of the digits.
   *
   * The order is from least significant digit to most significant digit, which
   * helps with the arithmetics.
   */
  std::vector<uint8_t> digits;
  /**
   * @brief Sign indicator.
   *
   * If true, the number is negative.
   */
  bool ne;

  /**
   * @brief Adds two bigints.
   * @param rhs The right-hand side bigint to add.
   * @return bigint representing *this + rhs, sign being the same as *this.
   */
  bigint add(const bigint &rhs) const {
    bigint result;
    result.ne = ne;
    size_t max_size = std::max(digits.size(), rhs.digits.size());
    result.digits.resize(max_size, 0);

    uint8_t c = 0;
    for (size_t i = 0; i < max_size || c > 0; i++) {
      uint8_t sum = c;
      if (i == result.digits.size())
        result.digits.push_back(0); // Last carry
      if (i < digits.size())
        sum += digits[i];
      if (i < rhs.digits.size())
        sum += rhs.digits[i];
      c = sum / 10;
      result.digits[i] = sum % 10;
    }

    result.normalize();
    return result;
  };

  /**
   * @brief Subtracts rhs from *this.
   * @param rhs The right-hand side bigint to subtract.
   * @return bigint representing *this - rhs.
   *
   * It finds the absolute different between the two bigints and determine the
   * sign of the result based on the absolute values and signs.
   */
  bigint sub(const bigint &rhs) const {
    bigint result;
    result.digits.clear(); // Remove the unused zero
    bool is_abs_less = abs_less(rhs);
    const bigint &larger = is_abs_less ? rhs : *this;
    const bigint &smaller = is_abs_less ? *this : rhs;

    if (ne != rhs.ne) {
      result.ne = ne;
    } else {
      result.ne = is_abs_less ? !ne : ne;
    }

    uint8_t b = 0;
    for (size_t i = 0; i < larger.digits.size(); i++) {
      int16_t tmp = larger.digits[i] - b -
                    (i < smaller.digits.size() ? smaller.digits[i] : 0);
      if (tmp < 0) {
        b = 1;
        tmp += 10;
      } else {
        b = 0;
      }
      result.digits.push_back(static_cast<uint8_t>(tmp));
    }

    result.normalize();
    return result;
  };

  /**
   * @brief Compares absolute values of *this and rhs.
   * @param rhs The right-hand side bigint.
   * @return true if |*this| < |rhs|, false otherwise.
   */
  bool abs_less(const bigint &rhs) const {
    // Length
    if (digits.size() != rhs.digits.size())
      return digits.size() < rhs.digits.size();
    // MSD -> LSD
    for (size_t i = digits.size(); i > 0; i--) {
      if (digits[i - 1] != rhs.digits[i - 1]) {
        return digits[i - 1] < rhs.digits[i - 1];
      }
    }
    // Eq
    return false;
  }

  /**
   * @brief Normalizes by removing leading zeros and fixing sign for
   * zero.
   */
  void normalize() {
    while (digits.size() > 1 && digits.back() == 0) {
      digits.pop_back();
    }

    if (digits.size() == 1 && digits[0] == 0) {
      ne = false;
    }
  }
};
