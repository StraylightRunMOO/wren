#ifndef pigeon_h
#define pigeon_h

/* Pigeon 0.5 — a class-based scripting language derived from Wren
 * (https://wren.io, MIT, Robert Nystrom and contributors).
 * This header is the public C API. <wren.h> is a one-release compatibility shim.
 */


#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// The Pigeon semantic version number components.
#define PIGEON_VERSION_MAJOR 0
#define PIGEON_VERSION_MINOR 5
#define PIGEON_VERSION_PATCH 0

// A human-friendly string representation of the version.
#define PIGEON_VERSION_STRING "0.5.0"

// A monotonically increasing numeric representation of the version number. Use
// this if you want to do range checks over versions.
#define PIGEON_VERSION_NUMBER (PIGEON_VERSION_MAJOR * 1000000 +                    \
                             PIGEON_VERSION_MINOR * 1000 +                       \
                             PIGEON_VERSION_PATCH)

#ifndef PIGEON_API
  #if defined(_MSC_VER) && defined(PIGEON_API_DLLEXPORT)
    #define PIGEON_API __declspec( dllexport )
  #else
    #define PIGEON_API
  #endif
#endif //PIGEON_API

// A single virtual machine for executing Pigeon code.
//
// Pigeon has no global state, so all state stored by a running interpreter lives
// here.
typedef struct PigeonVM PigeonVM;

// A handle to a Pigeon object.
//
// This lets code outside of the VM hold a persistent reference to an object.
// After a handle is acquired, and until it is released, this ensures the
// garbage collector will not reclaim the object it references.
typedef struct PigeonHandle PigeonHandle;

// A generic allocation function that handles all explicit memory management
// used by Pigeon. It's used like so:
//
// - To allocate new memory, [memory] is NULL and [newSize] is the desired
//   size. It should return the allocated memory or NULL on failure.
//
// - To attempt to grow an existing allocation, [memory] is the memory, and
//   [newSize] is the desired size. It should return [memory] if it was able to
//   grow it in place, or a new pointer if it had to move it.
//
// - To shrink memory, [memory] and [newSize] are the same as above but it will
//   always return [memory].
//
// - To free memory, [memory] will be the memory to free and [newSize] will be
//   zero. It should return NULL.
typedef void* (*PigeonReallocateFn)(void* memory, size_t newSize, void* userData);

// A function callable from Pigeon code, but implemented in C.
typedef void (*PigeonForeignMethodFn)(PigeonVM* vm);

// A finalizer function for freeing resources owned by an instance of a foreign
// class. Unlike most foreign methods, finalizers do not have access to the VM
// and should not interact with it since it's in the middle of a garbage
// collection.
typedef void (*PigeonFinalizerFn)(void* data);

// Called during compilation when an Object Number (#123) is encountered.
// [value] is the integer that follows the hash symbol.
// The callback should create the desired value and place it in slot 0.
// The value in slot 0 will be stored in the constant pool and loaded
// whenever this literal is evaluated.
//
// If this callback is NULL, Object Numbers will cause a compilation error.
typedef void (*PigeonObjectNumberFn)(PigeonVM* vm, int64_t value);

// Returns the default export name for a module (e.g., "Math" for "math").
// Returns NULL if the module has no default export.
typedef const char* (*PigeonResolveDefaultExportFn)(PigeonVM* vm, const char* name);

// Returns a NULL-terminated array of all export names for a module.
// Used for wildcard imports (e.g., `import "math" for *`).
// Returns NULL if the module is not found.
typedef const char** (*PigeonResolveExportsFn)(PigeonVM* vm, const char* name);

// Gives the host a chance to canonicalize the imported module name,
// potentially taking into account the (previously resolved) name of the module
// that contains the import. Typically, this is used to implement relative
// imports.
typedef const char* (*PigeonResolveModuleFn)(PigeonVM* vm,
    const char* importer, const char* name);

// Forward declare
struct PigeonLoadModuleResult;

// Called after loadModuleFn is called for module [name]. The original returned result
// is handed back to you in this callback, so that you can free memory if appropriate.
typedef void (*PigeonLoadModuleCompleteFn)(PigeonVM* vm, const char* name, struct PigeonLoadModuleResult result);

// The result of a loadModuleFn call. 
// [source] is the source code for the module, or NULL if the module is not found.
// [onComplete] an optional callback that will be called once Pigeon is done with the result.
typedef struct PigeonLoadModuleResult
{
  const char* source;
  PigeonLoadModuleCompleteFn onComplete;
  void* userData;
} PigeonLoadModuleResult;

// Loads and returns the source code for the module [name].
typedef PigeonLoadModuleResult (*PigeonLoadModuleFn)(PigeonVM* vm, const char* name);

// Returns a pointer to a foreign method on [className] in [module] with
// [signature].
typedef PigeonForeignMethodFn (*PigeonBindForeignMethodFn)(PigeonVM* vm,
    const char* module, const char* className, bool isStatic,
    const char* signature);

// Displays a string of text to the user.
typedef void (*PigeonWriteFn)(PigeonVM* vm, const char* text);

typedef enum
{
  // A syntax or resolution error detected at compile time.
  PIGEON_ERROR_COMPILE,

  // The error message for a runtime error.
  PIGEON_ERROR_RUNTIME,

  // One entry of a runtime error's stack trace.
  PIGEON_ERROR_STACK_TRACE
} PigeonErrorType;

// Reports an error to the user.
//
// An error detected during compile time is reported by calling this once with
// [type] `PIGEON_ERROR_COMPILE`, the resolved name of the [module] and [line]
// where the error occurs, and the compiler's error [message].
//
// A runtime error is reported by calling this once with [type]
// `PIGEON_ERROR_RUNTIME`, no [module] or [line], and the runtime error's
// [message]. After that, a series of [type] `PIGEON_ERROR_STACK_TRACE` calls are
// made for each line in the stack trace. Each of those has the resolved
// [module] and [line] where the method or function is defined and [message] is
// the name of the method or function.
typedef void (*PigeonErrorFn)(
    PigeonVM* vm, PigeonErrorType type, const char* module, int line,
    const char* message);

typedef struct
{
  // The callback invoked when the foreign object is created.
  //
  // This must be provided. Inside the body of this, it must call
  // [pigeonSetSlotNewForeign()] exactly once.
  PigeonForeignMethodFn allocate;

  // The callback invoked when the garbage collector is about to collect a
  // foreign object's memory.
  //
  // This may be `NULL` if the foreign class does not need to finalize.
  PigeonFinalizerFn finalize;
} PigeonForeignClassMethods;

// Returns a pair of pointers to the foreign methods used to allocate and
// finalize the data for instances of [className] in resolved [module].
typedef PigeonForeignClassMethods (*PigeonBindForeignClassFn)(
    PigeonVM* vm, const char* module, const char* className);

typedef struct
{
  // The callback Pigeon will use to allocate, reallocate, and deallocate memory.
  //
  // If `NULL`, defaults to a built-in function that uses `realloc` and `free`.
  PigeonReallocateFn reallocateFn;

  // The callback Pigeon uses to resolve a module name.
  //
  // Some host applications may wish to support "relative" imports, where the
  // meaning of an import string depends on the module that contains it. To
  // support that without baking any policy into Pigeon itself, the VM gives the
  // host a chance to resolve an import string.
  //
  // Before an import is loaded, it calls this, passing in the name of the
  // module that contains the import and the import string. The host app can
  // look at both of those and produce a new "canonical" string that uniquely
  // identifies the module. This string is then used as the name of the module
  // going forward. It is what is passed to [loadModuleFn], how duplicate
  // imports of the same module are detected, and how the module is reported in
  // stack traces.
  //
  // If you leave this function NULL, then the original import string is
  // treated as the resolved string.
  //
  // If an import cannot be resolved by the embedder, it should return NULL and
  // Pigeon will report that as a runtime error.
  //
  // Pigeon will take ownership of the string you return and free it for you, so
  // it should be allocated using the same allocation function you provide
  // above.
  PigeonResolveModuleFn resolveModuleFn;

  // The callback Pigeon uses to load a module.
  //
  // Since Pigeon does not talk directly to the file system, it relies on the
  // embedder to physically locate and read the source code for a module. The
  // first time an import appears, Pigeon will call this and pass in the name of
  // the module being imported. The method will return a result, which contains
  // the source code for that module. Memory for the source is owned by the 
  // host application, and can be freed using the onComplete callback.
  //
  // This will only be called once for any given module name. Pigeon caches the
  // result internally so subsequent imports of the same module will use the
  // previous source and not call this.
  //
  // If a module with the given name could not be found by the embedder, it
  // should return NULL and Pigeon will report that as a runtime error.
  PigeonLoadModuleFn loadModuleFn;

  // The callback Pigeon uses to find a foreign method and bind it to a class.
  //
  // When a foreign method is declared in a class, this will be called with the
  // foreign method's module, class, and signature when the class body is
  // executed. It should return a pointer to the foreign function that will be
  // bound to that method.
  //
  // If the foreign function could not be found, this should return NULL and
  // Pigeon will report it as runtime error.
  PigeonBindForeignMethodFn bindForeignMethodFn;

  // The callback Pigeon uses to find a foreign class and get its foreign methods.
  //
  // When a foreign class is declared, this will be called with the class's
  // module and name when the class body is executed. It should return the
  // foreign functions uses to allocate and (optionally) finalize the bytes
  // stored in the foreign object when an instance is created.
  PigeonBindForeignClassFn bindForeignClassFn;

  // The callback Pigeon uses to display text when `System.print()` or the other
  // related functions are called.
  //
  // If this is `NULL`, Pigeon discards any printed text.
  PigeonWriteFn writeFn;

  // The callback Pigeon uses to report errors.
  //
  // When an error occurs, this will be called with the module name, line
  // number, and an error message. If this is `NULL`, Pigeon doesn't report any
  // errors.
  PigeonErrorFn errorFn;

  // The callback Pigeon uses to resolve Object Numbers.
  //
  // When an Object Number like #123 is encountered during compilation,
  // this function is called with the integer value. It should return
  // a Pigeon Value (typically an object instance).
  //
  // If this is NULL, Object Numbers will cause a compilation error.
  PigeonObjectNumberFn objectNumberFn;

  // The callback Pigeon uses to resolve the default export for a module.
  //
  // When an import statement has no 'for' clause (e.g., `import "math"`),
  // this function is called to determine which variable to import by default.
  // It should return the name of the default export (e.g., "Math" for "math"),
  // or NULL if the module has no default export.
  //
  // If this is NULL, imports without 'for' clauses will cause a compilation error.
  PigeonResolveDefaultExportFn resolveDefaultExportFn;

  // The callback Pigeon uses to resolve all exports for wildcard imports.
  //
  // When an import statement uses '*' (e.g., `import "math" for *`),
  // this function is called to get the list of all public exports from the module.
  // It should return a NULL-terminated array of export names.
  // The returned array should remain valid for the duration of the import.
  //
  // If this is NULL, wildcard imports will cause a compilation error.
  PigeonResolveExportsFn resolveExportsFn;

  // The number of bytes Pigeon will allocate before triggering the first garbage
  // collection.
  //
  // If zero, defaults to 10MB.
  size_t initialHeapSize;

  // After a collection occurs, the threshold for the next collection is
  // determined based on the number of bytes remaining in use. This allows Pigeon
  // to shrink its memory usage automatically after reclaiming a large amount
  // of memory.
  //
  // This can be used to ensure that the heap does not get too small, which can
  // in turn lead to a large number of collections afterwards as the heap grows
  // back to a usable size.
  //
  // If zero, defaults to 1MB.
  size_t minHeapSize;

  // Pigeon will resize the heap automatically as the number of bytes
  // remaining in use after a collection changes. This number determines the
  // amount of additional memory Pigeon will use after a collection, as a
  // percentage of the current heap size.
  //
  // For example, say that this is 50. After a garbage collection, when there
  // are 400 bytes of memory still in use, the next collection will be triggered
  // after a total of 600 bytes are allocated (including the 400 already in
  // use.)
  //
  // Setting this to a smaller number wastes less memory, but triggers more
  // frequent garbage collections.
  //
  // If zero, defaults to 50.
  int heapGrowthPercent;

  // User-defined data associated with the VM.
  void* userData;

} PigeonConfiguration;

typedef enum
{
  PIGEON_RESULT_SUCCESS,
  PIGEON_RESULT_COMPILE_ERROR,
  PIGEON_RESULT_RUNTIME_ERROR
} PigeonInterpretResult;

// The type of an object stored in a slot.
//
// This is not necessarily the object's *class*, but instead its low level
// representation type.
typedef enum
{
  PIGEON_TYPE_BOOL,
  PIGEON_TYPE_NUM,
  PIGEON_TYPE_FOREIGN,
  PIGEON_TYPE_LIST,
  PIGEON_TYPE_MAP,
  PIGEON_TYPE_NULL,
  PIGEON_TYPE_STRING,

  // The object is of a type that isn't accessible by the C API.
  PIGEON_TYPE_UNKNOWN
} PigeonType;

// Get the current pigeon version number.
//
// Can be used to range checks over versions.
PIGEON_API int pigeonGetVersionNumber();

// Initializes [configuration] with all of its default values.
//
// Call this before setting the particular fields you care about.
PIGEON_API void pigeonInitConfiguration(PigeonConfiguration* configuration);

// Creates a new Pigeon virtual machine using the given [configuration]. Pigeon
// will copy the configuration data, so the argument passed to this can be
// freed after calling this. If [configuration] is `NULL`, uses a default
// configuration.
PIGEON_API PigeonVM* pigeonNewVM(PigeonConfiguration* configuration);

// Disposes of all resources is use by [vm], which was previously created by a
// call to [pigeonNewVM].
PIGEON_API void pigeonFreeVM(PigeonVM* vm);

// Immediately run the garbage collector to free unused memory.
PIGEON_API void pigeonCollectGarbage(PigeonVM* vm);

// Runs [source], a string of Pigeon source code in a new fiber in [vm] in the
// context of resolved [module].
PIGEON_API PigeonInterpretResult pigeonInterpret(PigeonVM* vm, const char* module,
                                  const char* source);

// Creates a handle that can be used to invoke a method with [signature] on
// using a receiver and arguments that are set up on the stack.
//
// This handle can be used repeatedly to directly invoke that method from C
// code using [pigeonCall].
//
// When you are done with this handle, it must be released using
// [pigeonReleaseHandle].
PIGEON_API PigeonHandle* pigeonMakeCallHandle(PigeonVM* vm, const char* signature);

// Calls [method], using the receiver and arguments previously set up on the
// stack.
//
// [method] must have been created by a call to [pigeonMakeCallHandle]. The
// arguments to the method must be already on the stack. The receiver should be
// in slot 0 with the remaining arguments following it, in order. It is an
// error if the number of arguments provided does not match the method's
// signature.
//
// After this returns, you can access the return value from slot 0 on the stack.
PIGEON_API PigeonInterpretResult pigeonCall(PigeonVM* vm, PigeonHandle* method);

// Releases the reference stored in [handle]. After calling this, [handle] can
// no longer be used.
PIGEON_API void pigeonReleaseHandle(PigeonVM* vm, PigeonHandle* handle);

// The following functions are intended to be called from foreign methods or
// finalizers. The interface Pigeon provides to a foreign method is like a
// register machine: you are given a numbered array of slots that values can be
// read from and written to. Values always live in a slot (unless explicitly
// captured using pigeonGetSlotHandle(), which ensures the garbage collector can
// find them.
//
// When your foreign function is called, you are given one slot for the receiver
// and each argument to the method. The receiver is in slot 0 and the arguments
// are in increasingly numbered slots after that. You are free to read and
// write to those slots as you want. If you want more slots to use as scratch
// space, you can call pigeonEnsureSlots() to add more.
//
// When your function returns, every slot except slot zero is discarded and the
// value in slot zero is used as the return value of the method. If you don't
// store a return value in that slot yourself, it will retain its previous
// value, the receiver.
//
// While Pigeon is dynamically typed, C is not. This means the C interface has to
// support the various types of primitive values a Pigeon variable can hold: bool,
// double, string, etc. If we supported this for every operation in the C API,
// there would be a combinatorial explosion of functions, like "get a
// double-valued element from a list", "insert a string key and double value
// into a map", etc.
//
// To avoid that, the only way to convert to and from a raw C value is by going
// into and out of a slot. All other functions work with values already in a
// slot. So, to add an element to a list, you put the list in one slot, and the
// element in another. Then there is a single API function pigeonInsertInList()
// that takes the element out of that slot and puts it into the list.
//
// The goal of this API is to be easy to use while not compromising performance.
// The latter means it does not do type or bounds checking at runtime except
// using assertions which are generally removed from release builds. C is an
// unsafe language, so it's up to you to be careful to use it correctly. In
// return, you get a very fast FFI.

// Returns the number of slots available to the current foreign method.
PIGEON_API int pigeonGetSlotCount(PigeonVM* vm);

// Ensures that the foreign method stack has at least [numSlots] available for
// use, growing the stack if needed.
//
// Does not shrink the stack if it has more than enough slots.
//
// It is an error to call this from a finalizer.
PIGEON_API void pigeonEnsureSlots(PigeonVM* vm, int numSlots);

// Gets the type of the object in [slot].
PIGEON_API PigeonType pigeonGetSlotType(PigeonVM* vm, int slot);

// Reads a boolean value from [slot].
//
// It is an error to call this if the slot does not contain a boolean value.
PIGEON_API bool pigeonGetSlotBool(PigeonVM* vm, int slot);

// Reads a byte array from [slot].
//
// The memory for the returned string is owned by Pigeon. You can inspect it
// while in your foreign method, but cannot keep a pointer to it after the
// function returns, since the garbage collector may reclaim it.
//
// Returns a pointer to the first byte of the array and fill [length] with the
// number of bytes in the array.
//
// It is an error to call this if the slot does not contain a string.
PIGEON_API const char* pigeonGetSlotBytes(PigeonVM* vm, int slot, int* length);

// Reads a number from [slot].
//
// It is an error to call this if the slot does not contain a number.
PIGEON_API double pigeonGetSlotDouble(PigeonVM* vm, int slot);

// Reads a foreign object from [slot] and returns a pointer to the foreign data
// stored with it.
//
// It is an error to call this if the slot does not contain an instance of a
// foreign class.
PIGEON_API void* pigeonGetSlotForeign(PigeonVM* vm, int slot);

// Reads a string from [slot].
//
// The memory for the returned string is owned by Pigeon. You can inspect it
// while in your foreign method, but cannot keep a pointer to it after the
// function returns, since the garbage collector may reclaim it.
//
// It is an error to call this if the slot does not contain a string.
PIGEON_API const char* pigeonGetSlotString(PigeonVM* vm, int slot);

// Creates a handle for the value stored in [slot].
//
// This will prevent the object that is referred to from being garbage collected
// until the handle is released by calling [pigeonReleaseHandle()].
PIGEON_API PigeonHandle* pigeonGetSlotHandle(PigeonVM* vm, int slot);

// Stores the boolean [value] in [slot].
PIGEON_API void pigeonSetSlotBool(PigeonVM* vm, int slot, bool value);

// Stores the array [length] of [bytes] in [slot].
//
// The bytes are copied to a new string within Pigeon's heap, so you can free
// memory used by them after this is called.
PIGEON_API void pigeonSetSlotBytes(PigeonVM* vm, int slot, const char* bytes, size_t length);

// Stores the numeric [value] in [slot].
PIGEON_API void pigeonSetSlotDouble(PigeonVM* vm, int slot, double value);

// Creates a new instance of the foreign class stored in [classSlot] with [size]
// bytes of raw storage and places the resulting object in [slot].
//
// This does not invoke the foreign class's constructor on the new instance. If
// you need that to happen, call the constructor from Pigeon, which will then
// call the allocator foreign method. In there, call this to create the object
// and then the constructor will be invoked when the allocator returns.
//
// Returns a pointer to the foreign object's data.
PIGEON_API void* pigeonSetSlotNewForeign(PigeonVM* vm, int slot, int classSlot, size_t size);

// Stores a new empty list in [slot].
PIGEON_API void pigeonSetSlotNewList(PigeonVM* vm, int slot);

// Stores a new empty map in [slot].
PIGEON_API void pigeonSetSlotNewMap(PigeonVM* vm, int slot);

// Stores null in [slot].
PIGEON_API void pigeonSetSlotNull(PigeonVM* vm, int slot);

// Stores the string [text] in [slot].
//
// The [text] is copied to a new string within Pigeon's heap, so you can free
// memory used by it after this is called. The length is calculated using
// [strlen()]. If the string may contain any null bytes in the middle, then you
// should use [pigeonSetSlotBytes()] instead.
PIGEON_API void pigeonSetSlotString(PigeonVM* vm, int slot, const char* text);

// Stores the value captured in [handle] in [slot].
//
// This does not release the handle for the value.
PIGEON_API void pigeonSetSlotHandle(PigeonVM* vm, int slot, PigeonHandle* handle);

// Returns the number of elements in the list stored in [slot].
PIGEON_API int pigeonGetListCount(PigeonVM* vm, int slot);

// Reads element [index] from the list in [listSlot] and stores it in
// [elementSlot].
PIGEON_API void pigeonGetListElement(PigeonVM* vm, int listSlot, int index, int elementSlot);

// Sets the value stored at [index] in the list at [listSlot], 
// to the value from [elementSlot]. 
PIGEON_API void pigeonSetListElement(PigeonVM* vm, int listSlot, int index, int elementSlot);

// Takes the value stored at [elementSlot] and inserts it into the list stored
// at [listSlot] at [index].
//
// As in Pigeon, negative indexes can be used to insert from the end. To append
// an element, use `-1` for the index.
PIGEON_API void pigeonInsertInList(PigeonVM* vm, int listSlot, int index, int elementSlot);

// Returns the number of entries in the map stored in [slot].
PIGEON_API int pigeonGetMapCount(PigeonVM* vm, int slot);

// Returns true if the key in [keySlot] is found in the map placed in [mapSlot].
PIGEON_API bool pigeonGetMapContainsKey(PigeonVM* vm, int mapSlot, int keySlot);

// Retrieves a value with the key in [keySlot] from the map in [mapSlot] and
// stores it in [valueSlot].
PIGEON_API void pigeonGetMapValue(PigeonVM* vm, int mapSlot, int keySlot, int valueSlot);

// Takes the value stored at [valueSlot] and inserts it into the map stored
// at [mapSlot] with key [keySlot].
PIGEON_API void pigeonSetMapValue(PigeonVM* vm, int mapSlot, int keySlot, int valueSlot);

// Removes a value from the map in [mapSlot], with the key from [keySlot],
// and place it in [removedValueSlot]. If not found, [removedValueSlot] is
// set to null, the same behaviour as the Pigeon Map API.
PIGEON_API void pigeonRemoveMapValue(PigeonVM* vm, int mapSlot, int keySlot,
                        int removedValueSlot);

// Looks up the top level variable with [name] in resolved [module] and stores
// it in [slot].
PIGEON_API void pigeonGetVariable(PigeonVM* vm, const char* module, const char* name,
                     int slot);

// Looks up the top level variable with [name] in resolved [module], 
// returns false if not found. The module must be imported at the time, 
// use pigeonHasModule to ensure that before calling.
PIGEON_API bool pigeonHasVariable(PigeonVM* vm, const char* module, const char* name);

// Returns true if [module] has been imported/resolved before, false if not.
PIGEON_API bool pigeonHasModule(PigeonVM* vm, const char* module);

// Sets the current fiber to be aborted, and uses the value in [slot] as the
// runtime error object.
PIGEON_API void pigeonAbortFiber(PigeonVM* vm, int slot);

// Returns the user data associated with the PigeonVM.
PIGEON_API void* pigeonGetUserData(PigeonVM* vm);

// Sets user data associated with the PigeonVM.
PIGEON_API void pigeonSetUserData(PigeonVM* vm, void* userData);

#endif
