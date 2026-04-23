// Test new import features - default imports and wildcard imports

// Test default import - import "math" should import math
import "math"
System.print(math is Class)  // expect: true
System.print(math.Pi > 3)    // expect: true

// Test default import - import "time" should import time
import "time"
System.print(time is Class)  // expect: true

// Test default import - import "meta" should import meta
import "meta"
System.print(meta is Class)  // expect: true

// Test default import - import "meta/reflection" should import reflection
import "meta/reflection"
System.print(reflection is Class)  // expect: true

// Test wildcard import with a module not yet loaded
// Import secondary symbols from math via explicit names
import "math" for Rand, Stats, E, Pi
System.print(Rand is Class)  // expect: true
System.print(Stats is Class) // expect: true
System.print(E > 2)          // expect: true

// Test wildcard import - import "time" for *
// (time already imported; Duration, Timer, Now, Sleep not yet bound)
import "time" for Duration, Timer
System.print(Duration is Class) // expect: true
System.print(Timer is Class)    // expect: true

// Test regular import still works
import "strconv" for strconv
System.print(strconv is Class)  // expect: true

// Test import with alias still works
import "strings" for strings as str
System.print(str is Class)  // expect: true

System.print("Import tests passed!")  // expect: Import tests passed!
