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
float version = 0.41
boolean is_awesome = true
number meaning = 42
items = [1, 2, 3, 4, 5]
config = {"debug": true, "port": 8080}
nil_val = nil

print("  Language: " + name + " v" + fun_version())
print("  Awesome: " + to_string(is_awesome))
print("  Nil: " + to_string(nil_val))
print("")

// ============================================
// 2. Type Introspection
// ============================================
print("2. Type Introspection:")
number check_int = 42
string check_str = "hello"
check_arr = [1, 2, 3]
check_map = {"key": "value"}
check_float = 3.14

print("  typeof(42) = " + typeof(check_int))
print("  typeof(\"hello\") = " + typeof(check_str))
print("  typeof([1,2,3]) = " + typeof(check_arr))
print("  typeof(map) = " + typeof(check_map))
print("  typeof(3.14) = " + typeof(check_float))
print("")

// ============================================
// 3. Conversion & Casting
// ============================================
print("3. Conversion & Casting:")
print("  to_string(42) = " + to_string(42))
print("  to_number(\"99\") = " + to_string(to_number("99")))
casted = cast(1, "boolean")
print("  cast(1, \"boolean\") = " + to_string(casted))
casted2 = cast("42", "number")
print("  cast(\"42\", \"number\") = " + to_string(casted2))
print("")

// ============================================
// 4. String Manipulation
// ============================================
print("4. String Operations:")
string text = "Hello, Fun Language!"
print("  Original: " + text)
print("  Length: " + to_string(len(text)))
print("  Substr(0,5): " + substr(text, 0, 5))
print("  Find(\"Fun\"): " + to_string(find(text, "Fun")))
parts = split(text, " ")
print("  Split by space: " + to_string(parts))
joined = join(parts, "-")
print("  Join with '-': " + joined)
print("")

// ============================================
// 5. Modern Array Operations
// ============================================
print("5. Array Operations:")
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
print("  Original: " + to_string(numbers))
print("  Length: " + to_string(len(numbers)))
print("  Index 0: " + to_string(numbers[0]))
print("  Index last: " + to_string(numbers[len(numbers) - 1]))

// Slice syntax arr[start:end]
sliced = numbers[2:7]
print("  Slice [2:7]: " + to_string(sliced))

// Mutating array ops
arr = [10, 20, 30]
push(arr, 40)
print("  After push(40): " + to_string(arr))
removed = pop(arr)
print("  Popped: " + to_string(removed) + ", arr: " + to_string(arr))
insert(arr, 1, 15)
print("  After insert(1, 15): " + to_string(arr))
removed = remove(arr, 0)
print("  Removed at 0: " + to_string(removed) + ", arr: " + to_string(arr))

// Search & utility
print("  Contains 20? " + to_string(contains(arr, 20)))
print("  Index of 20? " + to_string(indexOf(arr, 20)))
print("  Enumerate: " + to_string(enumerate(["a", "b", "c"])))
print("  Zip: " + to_string(zip([1, 2, 3], ["x", "y", "z"])))
clear(arr)
print("  After clear: " + to_string(arr))
print("")

// ============================================
// 6. Maps (Dictionaries)
// ============================================
print("6. Map Operations:")
person = {"name": "Alice", "age": 30, "role": "Developer"}

print("  Person: " + to_string(person))
print("  Has 'age' key: " + to_string(has(person, "age")))
print("  Keys: " + to_string(keys(person)))
print("  Values: " + to_string(values(person)))

// Bracket access and assignment
print("  person[\"name\"]: " + person["name"])
person["age"] = 31
print("  Updated age: " + to_string(person["age"]))

// Object property access (dot notation)
print("  person.name: " + person.name)
print("")

// ============================================
// 7. Object-Oriented Programming
// ============================================
print("7. Classes & Objects:")

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
// 8. Inheritance
// ============================================
print("8. Inheritance:")

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
// 9. Error Handling & Exceptions
// ============================================
print("9. Exception Handling:")

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
// 10. Comparison & Logical Operators
// ============================================
print("10. Comparison & Logical Operators:")
number a = 10
number b = 20
print("  a=10, b=20")
print("  a < b: " + to_string(a < b))
print("  a <= b: " + to_string(a <= b))
print("  a > b: " + to_string(a > b))
print("  a >= b: " + to_string(a >= b))
print("  a == b: " + to_string(a == b))
print("  a != b: " + to_string(a != b))
print("  a < b && a > 0: " + to_string(a < b && a > 0))
print("  a > b || a > 0: " + to_string(a > b || a > 0))
print("  !true: " + to_string(!true))
print("  Ternary (a < b ? \"yes\" : \"no\"): " + (a < b ? "yes" : "no"))
print("")

// ============================================
// 11. Control Flow: if/else if/else
// ============================================
print("11. If/Else If/Else:")
number score = 85

if score >= 90
  print("  Grade: A")
else if score >= 80
  print("  Grade: B")
else if score >= 70
  print("  Grade: C")
else
  print("  Grade: F")
print("")

// ============================================
// 12. Control Flow: Loops
// ============================================
print("12. Loop Variants:")

// For-each with array
print("  For-each array:")
for item in ["apple", "banana", "cherry"]
  print("    " + item)

// For-range loop
print("  For-range 0..4:")
for i in range(0, 5)
  print("    " + to_string(i))

// For-map loop
print("  For-map key, value:")
config_map = {"a": 1, "b": 2, "c": 3}
for (k, v) in config_map
  print("    " + k + " = " + to_string(v))

// While with break/continue
print("  While with break/continue:")
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
// 13. Mathematical Operations
// ============================================
print("13. Math Functions:")
number mx = -16
print("  abs(-16) = " + to_string(abs(mx)))
print("  min(10, 20) = " + to_string(min(10, 20)))
print("  max(10, 20) = " + to_string(max(10, 20)))
print("  clamp(50, 0, 10) = " + to_string(clamp(50, 0, 10)))
print("  pow(2, 10) = " + to_string(pow(2, 10)))
print("  sqrt(144) = " + to_string(sqrt(144)))
print("  floor(3.7) = " + to_string(floor(3.7)))
print("  ceil(3.2) = " + to_string(ceil(3.2)))
print("  round(3.5) = " + to_string(round(3.5)))
print("  trunc(3.9) = " + to_string(trunc(3.9)))
print("  sin(0) = " + to_string(sin(0)))
print("  cos(0) = " + to_string(cos(0)))
print("  gcd(12, 8) = " + to_string(gcd(12, 8)))
print("  lcm(12, 8) = " + to_string(lcm(12, 8)))
print("  isqrt(50) = " + to_string(isqrt(50)))
print("  sign(-42) = " + to_string(sign(-42)))
print("  fmin(3.1, 2.9) = " + to_string(fmin(3.1, 2.9)))
print("  fmax(3.1, 2.9) = " + to_string(fmax(3.1, 2.9)))

// Random (deterministic for demo)
random_seed(42)
print("  random_int(1, 100) = " + to_string(random_int(1, 100)))
print("  random_number(8) = " + random_number(8))
print("")

// ============================================
// 14. Bitwise Operations
// ============================================
print("14. Bitwise Operations:")
number bits1 = 12  // 1100
number bits2 = 10  // 1010
print("  12 & 10 = " + to_string(band(bits1, bits2)))
print("  12 | 10 = " + to_string(bor(bits1, bits2)))
print("  12 ^ 10 = " + to_string(bxor(bits1, bits2)))
print("  ~12 = " + to_string(bnot(bits1)))
print("  12 << 2 = " + to_string(shl(bits1, 2)))
print("  12 >> 2 = " + to_string(shr(bits1, 2)))
print("  rol(0x80000001, 1) = " + to_string(rol(0x80000001, 1)))
print("  ror(0x80000001, 1) = " + to_string(ror(0x80000001, 1)))
print("")

// ============================================
// 15. Functional & Higher-Order Programming
// ============================================
print("15. Higher-Order Functions:")

fun double(n)
  return n * 2

fun apply_twice(x, func)
  return func(func(x))

result = apply_twice(5, double)
print("  apply_twice(5, double) = " + to_string(result))
print("")

// ============================================
// 16. Array Higher-Order Functions
// ============================================
print("16. Array Higher-Order Functions:")
nums = [1, 2, 3, 4, 5]

fun square(x)
  return x * x

squared = map(nums, square)
print("  map(nums, square): " + to_string(squared))

fun is_even(x)
  return x % 2 == 0

evens = filter(nums, is_even)
print("  filter(nums, is_even): " + to_string(evens))

fun sum(acc, x)
  return acc + x

total = reduce(nums, 0, sum)
print("  reduce(nums, 0, sum): " + to_string(total))
print("")

// ============================================
// 17. File I/O
// ============================================
print("17. File I/O:")
write_file("/tmp/fun_demo.txt", "Hello from Fun!")
content = read_file("/tmp/fun_demo.txt")
print("  Written and read back: " + content)
print("")

// ============================================
// 18. Environment & OS
// ============================================
print("18. Environment & OS:")
print("  HOME: " + env("HOME"))
print("  Version: " + fun_version())

// List directory (non-empty /tmp assumed)
listing = os_list_dir("/tmp")
print("  /tmp has " + to_string(len(listing)) + " entries")

// Run a command and capture output
proc_result = proc_run("echo hello from fun")
print("  proc_run output: " + proc_result["out"])

// System call exit code
sys_code = system("true")
print("  system(\"true\") exit: " + to_string(sys_code))
print("")

// ============================================
// 19. Date, Time & Sleep
// ============================================
print("19. Date, Time & Sleep:")
now = time_now_ms()
print("  Now (epoch ms): " + to_string(now))
print("  Date formatted: " + date_format(now, "%Y-%m-%d %H:%M:%S"))

mono = clock_mono_ms()
print("  Monotonic ms: " + to_string(mono))

// Short sleep to demonstrate
sleep(10)
after = clock_mono_ms()
diff = after - mono
print("  Slept 10 ms, elapsed: " + to_string(diff) + " ms")
print("")

// ============================================
// 20. Echo (print without newline)
// ============================================
print("20. Echo (no newline):")
echo("  Hello, ")
echo("world")
print("!")
print("")

// ============================================
// 21. Threading
// ============================================
print("21. Threading:")

fun worker(id)
  print("    Thread " + to_string(id) + " says hi!")
  return id * 2

t1 = thread_spawn(worker, [1])
t2 = thread_spawn(worker, [2])
r1 = thread_join(t1)
r2 = thread_join(t2)
print("  Joined thread results: " + to_string(r1) + ", " + to_string(r2))
print("")

// ============================================
// 22. Type-Safe Integer Clamping
// ============================================
print("22. Integer Clamping:")
byte val = 300
print("  byte val = 300 -> clamped to " + to_string(val))
int8 signed = 200
print("  int8 val = 200 -> clamped to " + to_string(signed))
uint16 big = 70000
print("  uint16 val = 70000 -> clamped to " + to_string(big))
print("")

// ============================================
// Conclusion
// ============================================
print("=== Feature Showcase Complete! ===")
print("Fun combines modern language features with simplicity.")
print("Explore more examples in ./examples/ directory!")
