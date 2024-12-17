#include "bigint.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

int main() {
  size_t total = 0;
  size_t passed = 0;

  auto test = [&](const std::string &desc, auto f) {
    total++;
    try {
      f();
      std::cout << "[PASS] " << desc << "\n";
      passed++;
    } catch (const std::exception &e) {
      std::cout << "[FAIL] " << desc << " - Exception: " << e.what() << "\n";
    } catch (...) {
      std::cout << "[FAIL] " << desc << " - Unknown exception\n";
    }
  };

  test("Default constructor", [&]() {
    bigint a;
    if (a != bigint(0))
      throw std::runtime_error("Default constructor should create zero.");
  });

  test("int64_t constructor", [&]() {
    bigint a(-1111);
    if (!(a == bigint(-1111)))
      throw std::runtime_error("int64_t constructor failed.");
  });

  test("String constructor", [&]() {
    bigint a(1111);
    std::ostringstream oss;
    oss << a;
    if (oss.str() != "1111")
      throw std::runtime_error("String constructor failed.");
  });

  test("String constructor with negative number", [&]() {
    bigint a(-1111);
    std::ostringstream oss;
    oss << a;
    if (oss.str() != "-1111")
      throw std::runtime_error("String constructor failed.");
  });

  test("String constructor with invalid input", [&]() {
    bool e = false;
    try {
      bigint a("a1");
    } catch (const std::invalid_argument &) {
      e = true;
    }
    if (!e)
      throw std::runtime_error("Didn't throw exception upon invalid input.");
  });

  test("+", [&]() {
    bigint a(1111);
    bigint b(1);
    bigint c = a + b;
    if (c != bigint(1112))
      throw std::runtime_error("+ failed.");
  });

  test("+ with negative numbers", [&]() {
    bigint a(-10);
    bigint b(-20);
    bigint c = a + b;
    if (c != bigint(-30))
      throw std::runtime_error("+ with negative numbers failed.");
  });

  test("-", [&]() {
    bigint a("1000000000000000000");
    bigint b(1);
    bigint c = a - b;
    if (c != bigint("999999999999999999"))
      throw std::runtime_error("- failed.");
  });

  test("- with negative result", [&]() {
    bigint a(10);
    bigint b(20);
    bigint c = a - b;
    if (c != bigint(-10))
      throw std::runtime_error("- with negative result failed.");
  });

  test("*", [&]() {
    bigint a(1111111111);
    bigint b(2222222222);
    bigint c = a * b;
    if (c != bigint("2469135801975308642"))
      throw std::runtime_error("* failed.");
  });

  test("negatives *", [&]() {
    bigint a(-2222222222);
    bigint b(1111111111);
    bigint c = a * b;
    if (c != bigint("-2469135801975308642"))
      throw std::runtime_error("negative * failed.");
  });

  test("+=", [&]() {
    bigint a(999999);
    a += bigint(1);
    if (a != bigint(1000000))
      throw std::runtime_error("+= failed.");
  });

  test("-=", [&]() {
    bigint a(1000000);
    a -= bigint(1);
    if (a != bigint(999999))
      throw std::runtime_error("-= failed.");
  });

  test("*=", [&]() {
    bigint a(1000);
    a *= bigint(1000);
    if (a != bigint(1000000))
      throw std::runtime_error("*= failed.");
  });

  test("negation", [&]() {
    bigint a(123);
    bigint b = -a;
    if (b != bigint(-123))
      throw std::runtime_error("Negation failed.");
  });

  test("==", [&]() {
    bigint a(12333);
    bigint b(12333);
    if (!(a == b))
      throw std::runtime_error("== failed.");
  });

  test("!=", [&]() {
    bigint a(12333);
    bigint b(12332);
    if (!(a != b))
      throw std::runtime_error("!= failed for different numbers.");
  });

  test("<", [&]() {
    bigint a(1);
    bigint b(2);
    if (!(a < b))
      throw std::runtime_error("< failed.");
  });

  test("<=", [&]() {
    bigint a(1);
    bigint b(1);
    if (!(a <= b))
      throw std::runtime_error("<= failed for equal numbers.");
  });

  test(">", [&]() {
    bigint a(2);
    bigint b(1);
    if (!(a > b))
      throw std::runtime_error("> failed.");
  });

  test(">=", [&]() {
    bigint a(1);
    bigint b(1);
    if (!(a >= b))
      throw std::runtime_error(">= failed.");
  });

  test("<<", [&]() {
    bigint a(-42);
    std::ostringstream oss;
    oss << a;
    if (oss.str() != "-42")
      throw std::runtime_error("<< failed.");
  });

  test("Pre-increment ++", [&]() {
    bigint a(999);
    ++a;
    if (a != bigint(1000))
      throw std::runtime_error("Pre-increment failed.");
  });

  test("Post-increment ++", [&]() {
    bigint a(999);
    bigint b = a++;
    if (b != bigint(999) || a != bigint(1000))
      throw std::runtime_error("Post-increment failed.");
  });

  test("Pre-decrement --", [&]() {
    bigint a(1000);
    --a;
    if (a != bigint(999))
      throw std::runtime_error("Pre-decrement failed.");
  });

  test("Post-decrement --", [&]() {
    bigint a(1000);
    bigint b = a--;
    if (b != bigint(1000) || a != bigint(999))
      throw std::runtime_error("Post-decrement failed.");
  });

  std::cout << "Total: " << total << "\n";
  std::cout << "Passed: " << passed << "\n";
  std::cout << "Failed: " << (total - passed) << "\n";
}
