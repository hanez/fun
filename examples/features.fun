#!/usr/bin/env fun
// features.fun - Showcase of Fun's neat features
// Demonstrates modern language capabilities in pure Fun

print("=== Fun Language Feature Showcase ===")
print("")

// ============================================
// 1. Type System & Type Safety
// ============================================
print("1. Strong Type System:")
string name = "Fun Language"
float version = 0.3
boolean is_awesome = true
items = [1, 2, 3, 4, 5]
config = {"debug": true, "port": 8080}

print("  Language: " + name + " v" + to_string(version))
print("  Awesome: " + to_string(is_awesome))
print("")

// ============================================
// 2. Modern Array Operations
// ============================================
print("2. Array Operations:")
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
print("  Original: " + to_string(numbers))

// Array helpers from spec: push, join, map, filter, reduce
joined = join([10, 20, 30], ", ")
print("  Joined: " + joined)

// Iterate arrays
print("  Iteration:")
for item in ["apple", "banana", "cherry"]
  print("    " + item)
print("")

// ============================================
// 3. Maps (Dictionaries)
// ============================================
print("3. Map Operations:")
person = {"name": "Alice", "age": 30, "role": "Developer"}

print("  Person: " + to_string(person))
print("  Has 'age' key: " + to_string(has(person, "age")))
print("  Keys: " + to_string(keys(person)))
print("  Values: " + to_string(values(person)))
print("")

// ============================================
// 4. Object-Oriented Programming
// ============================================
print("4. Classes & Objects:")

class Counter(number initial, string label)
  count = 0
  name = ""

  fun _construct(this, initial, label)
    this.count = initial
    this.name = label

  fun increment(this)
    this.count = this.count + 1

  fun get_value(this)
    return this.count

  fun display(this)
    print("  " + this.name + ": " + to_string(this.count))

counter = Counter(100, "Score")
counter.display()
counter.increment()
counter.increment()
counter.display()
print("")

// ============================================
// 5. Inheritance
// ============================================
print("5. Inheritance:")

class Animal(string type)
  species = ""

  fun _construct(this, type)
    this.species = type

  fun speak(this)
    return "Some sound"

class Dog(string dog_name) extends Animal
  name = ""

  fun _construct(this, dog_name)
    this.species = "Canine"
    this.name = dog_name

  fun speak(this)
    return "Woof! I'm " + this.name

dog = Dog("Buddy")
print("  " + dog.speak())
print("  Species: " + dog.species)
print("")

// ============================================
// 6. Error Handling
// ============================================
print("6. Exception Handling:")

try
  print("  Attempting risky operation...")
  result = 42 / 2
  print("  Result: " + to_string(result))
catch err
  print("  Caught error! Handled gracefully.")
finally
  print("  Cleanup completed.")
print("")

// ============================================
// 7. String Manipulation
// ============================================
print("7. String Features:")
string text = "Hello, Fun Language!"
print("  Original: " + text)
// Note: Using stdlib functions (assumed to exist in utils modules)
// len, substr, find, split would come from stdlib
print("")

// ============================================
// 8. Mathematical Operations
// ============================================
print("8. Math Functions:")
float x = 16.7
print("  x = " + to_string(x))
// Note: Math functions like sqrt, floor, ceil, abs, gcd, lcm
// would come from stdlib <utils/math.fun> or similar
print("")

// ============================================
// 9. Bitwise Operations
// ============================================
print("9. Bitwise Operations:")
number bits1 = 12
number bits2 = 10
print("  12 & 10 = " + to_string(band(bits1, bits2)))
print("  12 | 10 = " + to_string(bor(bits1, bits2)))
print("  12 ^ 10 = " + to_string(bxor(bits1, bits2)))
print("  12 << 2 = " + to_string(shl(bits1, 2)))
print("  ~12 = " + to_string(bnot(bits1)))
print("")

// ============================================
// 10. Control Flow
// ============================================
print("10. Control Flow:")

// For loop with array
print("  Countdown:")
for i in [5, 4, 3, 2, 1]
  print("    " + to_string(i) + "...")
print("    Liftoff!")

// While with break/continue
print("  Skip evens:")
number n = 0
while n < 10
  n = n + 1
  if n % 2 == 0
    continue
  print("    " + to_string(n))
  if n >= 7
    break
print("")

// ============================================
// 11. Type Introspection
// ============================================
print("11. Type Introspection:")
number check_int = 42
string check_str = "hello"
check_arr = [1, 2, 3]
check_map = {"key": "value"}

print("  typeof(42) = " + typeof(check_int))
print("  typeof(\"hello\") = " + typeof(check_str))
print("  typeof([1,2,3]) = " + typeof(check_arr))
print("  typeof(map) = " + typeof(check_map))
print("")

// ============================================
// 12. Functional Programming
// ============================================
print("12. Higher-Order Functions:")

fun double(n)
  return n * 2

fun apply_twice(x, func)
  return func(func(x))

result = apply_twice(5, double)
print("  apply_twice(5, double) = " + to_string(result))
print("")

// ============================================
// 13. Array Operations with Spec Functions
// ============================================
print("13. Array Higher-Order Functions:")
nums = [1, 2, 3, 4, 5]

fun square(x)
  return x * x

squared = map(nums, square)
print("  Squared: " + to_string(squared))

fun is_even(x)
  return x % 2 == 0

evens = filter(nums, is_even)
print("  Evens: " + to_string(evens))

fun sum(acc, x)
  return acc + x

total = reduce(nums, 0, sum)
print("  Sum: " + to_string(total))
print("")

// ============================================
// Conclusion
// ============================================
print("=== Feature Showcase Complete! ===")
print("Fun combines modern language features with simplicity.")
print("Explore more examples in ./examples/ directory!")
