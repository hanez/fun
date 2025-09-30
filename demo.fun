#!/usr/bin/env fun

// Fun Interactive Demo
// Run: FUN_LIB_DIR="$(pwd)/lib" ./build/fun demo.fun   (Linux/macOS/FreeBSD)
//      $env:FUN_LIB_DIR="$PWD\lib"; .\build\fun.exe demo.fun  (Windows PowerShell)

// Use a stdlib helper from the repository (fallback to ./lib via preprocessor)
#include <utils/math.fun>

print("")
print("=== Fun Interactive Demo ===")

print("")
print("== Basics: dynamic vs typed variables and typeof ==")
x = 123
print("x=" + to_string(x) + " typeof=" + typeof(x))
x = "hello"
print("x=" + to_string(x) + " typeof=" + typeof(x))

number n = 42
print("n=" + to_string(n) + " typeof=" + typeof(n))
n = n + 8
print("n=" + to_string(n) + " typeof=" + typeof(n))
// Uncomment to see a runtime type error and halt the program:
// n = "oops"

boolean flag = 0
print("flag=" + to_string(flag) + " typeof=" + typeof(flag))
flag = 2     // gets clamped to 1
print("flag(after clamp 2)=" + to_string(flag))

int8 si = 130     // clamps to 127
uint8 u8 = 300    // clamps to 255
print("int8 si=" + to_string(si) + ", uint8 u8=" + to_string(u8))

nil nothing = nil
print("nothing typeof=" + typeof(nothing))

print("")
print("== Arithmetic, comparisons, and logic ==")
print("3+4=" + to_string(3 + 4))
print("10-3=" + to_string(10 - 3))
print("6*7=" + to_string(6 * 7))
print("20/3=" + to_string(20 / 3))
print("20%3=" + to_string(20 % 3))
print("2<3=" + to_string(2 < 3) + ", 3<=3=" + to_string(3 <= 3))
print("5>2=" + to_string(5 > 2) + ", 2>=2=" + to_string(2 >= 2))
print("2==2=" + to_string(2 == 2) + ", 2!=3=" + to_string(2 != 3))
print("(1 && 0)=" + to_string(1 && 0) + ", (0 || 1)=" + to_string(0 || 1) + ", !0=" + to_string(!0))

print("")
print("== Strings and arrays ==")
s = "alpha,beta,gamma"
parts = split(s, ",")
print("split -> len=" + to_string(len(parts)))
print("join(parts,'|')=" + join(parts, "|"))
print("substr('abcdef', 2, 3)=" + substr("abcdef", 2, 3))
print("find('hello world','world')=" + to_string(find("hello world", "world")))

arr = [1, 2, 3]
print("arr len=" + to_string(len(arr)))
push(arr, 4)
print("after push 4 -> len=" + to_string(len(arr)))
v = pop(arr)
print("popped=" + to_string(v) + " len=" + to_string(len(arr)))
insert(arr, 1, 99)  // [1,99,2,3]
print("after insert(1,99) arr[1]=" + to_string(arr[1]))
set(arr, 2, 55)     // [1,99,55,3]
print("after set(2,55) arr[2]=" + to_string(arr[2]))
print("contains(arr, 55)=" + to_string(contains(arr, 55)) + ", indexOf(arr, 99)=" + to_string(indexOf(arr, 99)))
print("slice arr[1:3] len=" + to_string(len(arr[1:3])))
print("join(arr, ',')=" + join(arr, ","))

print("")
print("== Enumerate and zip ==")
for p in enumerate(["a", "b", "c"])
  print("idx=" + to_string(p[0]) + " val=" + to_string(p[1]))
z = zip([1, 2], ["x", "y"])
for pair in z
  print("(" + to_string(pair[0]) + "," + to_string(pair[1]) + ")")

print("")
print("== Maps (dictionaries) ==")
m = {"name": "Alice", "age": 30}
print("has(m,'age')=" + to_string(has(m, "age")))
print("m['name']=" + to_string(m["name"]))
m.age = 31
print("m.age after = " + to_string(m.age))
print("keys: " + join(keys(m), ","))
print("values count=" + to_string(len(values(m))))

print("")
print("== Functions and higher-order ops (map/filter/reduce) ==")
fun greet(name)
  print("Hello, " + to_string(name) + "!")
greet("Fun")

fun double(x)
  return x * 2

nums = [1, 2, 3, 4, 5]
twice = map(nums, double)
print("len(map)=" + to_string(len(twice)) + " first=" + to_string(twice[0]))

fun isEven(x)
  return (x % 2) == 0
evens = filter(nums, isEven)
print("filter evens len=" + to_string(len(evens)))

fun sum(acc, x)
  return acc + x
total = reduce(nums, 0, sum)
print("reduce sum=" + to_string(total))

print("")
print("== If / else-if / else and loops ==")
val = 7
if (val < 0)
  print("neg")
else if (val == 0)
  print("zero")
else
  print("pos")

print("for range(0, 5):")
for i in range(0, 5)
  print(i)

print("for in array:")
for x in ["h", "i", "!"]
  print(x)

print("while loop (count to 3):")
c = 0
while (c < 3)
  print(c)
  c = c + 1

print("")
print("== Classes (with 'this' and methods) ==")
class Person(string name, number age)
  // default field values (can be overridden by constructor params)
  full = name + " (" + to_string(age) + ")"

  // required: first param is 'this'
  fun say(this)
    print("I am " + to_string(this.full))

  // typeof(instance) will return this string for Maps tagged with __class
  fun toString(this)
    return "Person"

p = Person("Alice", 30)
p.say()
print("typeof p: " + typeof(p))

print("")
print("== Math and bitwise helpers ==")
print("min(3,9)=" + to_string(min(3, 9)) + ", max(3,9)=" + to_string(max(3, 9)))
print("clamp(15,0,10)=" + to_string(clamp(15, 0, 10)) + ", abs(-5)=" + to_string(abs(-5)))
print("pow(2,10)=" + to_string(pow(2, 10)))
print("band(0xF0,0x3C)=" + to_string(band(0xF0, 0x3C)))
print("bor(0x0F,0x30)=" + to_string(bor(0x0F, 0x30)))
print("bxor(0xFF,0x0F)=" + to_string(bxor(0xFF, 0x0F)))
print("bnot(0x0F)=" + to_string(bnot(0x0F)))
print("shl(1,4)=" + to_string(shl(1, 4)) + ", shr(128,3)=" + to_string(shr(128, 3)))
print("rol(0x12,1)=" + to_string(rol(0x12, 1)) + ", ror(0x12,1)=" + to_string(ror(0x12, 1)))

print("")
print("== Random numbers ==")
random(12345)  // seed
print("randomInt(1,10) -> " + to_string(randomInt(1, 10)))

print("")
print("== File IO and environment ==")
write_ok = write_file("demo_tmp.txt", "Hello from Fun!\n")
print("write_file ok=" + to_string(write_ok))
content = read_file("demo_tmp.txt")
print("read_file len=" + to_string(len(content)))
print("PATH starts with: " + substr(env("PATH"), 0, 24))

print("")
print("== Library include demo ==")
print("add(2,3) from utils/math.fun -> " + to_string(add(2, 3)))
print("times(4,5) from utils/math.fun -> " + to_string(times(4, 5)))

print("")
print("=== Demo complete. Have Fun! ===")
