#ifndef wren_h
#define wren_h

/* Compatibility shim for one release. Include <pigeon.h> in new code.
 * Dropped when Pigeon 1.0 is tagged. */
#ifndef PIGEON_NO_WREN_DEPRECATION
#warning "wren.h is deprecated; include pigeon.h"
#endif

#include "pigeon.h"

#define WREN_VERSION_MAJOR   PIGEON_VERSION_MAJOR
#define WREN_VERSION_MINOR   PIGEON_VERSION_MINOR
#define WREN_VERSION_PATCH   PIGEON_VERSION_PATCH
#define WREN_VERSION_STRING  PIGEON_VERSION_STRING
#define WREN_VERSION_NUMBER  PIGEON_VERSION_NUMBER

#ifndef WREN_API
#define WREN_API PIGEON_API
#endif

#define WrenVM                      PigeonVM
#define WrenHandle                  PigeonHandle
#define WrenReallocateFn            PigeonReallocateFn
#define WrenForeignMethodFn         PigeonForeignMethodFn
#define WrenFinalizerFn             PigeonFinalizerFn
#define WrenObjectNumberFn          PigeonObjectNumberFn
#define WrenResolveDefaultExportFn  PigeonResolveDefaultExportFn
#define WrenResolveExportsFn        PigeonResolveExportsFn
#define WrenResolveModuleFn         PigeonResolveModuleFn
#define WrenLoadModuleCompleteFn    PigeonLoadModuleCompleteFn
#define WrenLoadModuleResult        PigeonLoadModuleResult
#define WrenLoadModuleFn            PigeonLoadModuleFn
#define WrenBindForeignMethodFn     PigeonBindForeignMethodFn
#define WrenWriteFn                 PigeonWriteFn
#define WrenErrorType               PigeonErrorType
#define WrenErrorFn                 PigeonErrorFn
#define WrenForeignClassMethods     PigeonForeignClassMethods
#define WrenBindForeignClassFn      PigeonBindForeignClassFn
#define WrenConfiguration           PigeonConfiguration
#define WrenInterpretResult         PigeonInterpretResult
#define WrenType                    PigeonType

#define WREN_ERROR_COMPILE      PIGEON_ERROR_COMPILE
#define WREN_ERROR_RUNTIME      PIGEON_ERROR_RUNTIME
#define WREN_ERROR_STACK_TRACE  PIGEON_ERROR_STACK_TRACE

#define WREN_RESULT_SUCCESS        PIGEON_RESULT_SUCCESS
#define WREN_RESULT_COMPILE_ERROR  PIGEON_RESULT_COMPILE_ERROR
#define WREN_RESULT_RUNTIME_ERROR  PIGEON_RESULT_RUNTIME_ERROR

#define WREN_TYPE_BOOL     PIGEON_TYPE_BOOL
#define WREN_TYPE_NUM      PIGEON_TYPE_NUM
#define WREN_TYPE_FOREIGN  PIGEON_TYPE_FOREIGN
#define WREN_TYPE_LIST     PIGEON_TYPE_LIST
#define WREN_TYPE_MAP      PIGEON_TYPE_MAP
#define WREN_TYPE_NULL     PIGEON_TYPE_NULL
#define WREN_TYPE_STRING   PIGEON_TYPE_STRING
#define WREN_TYPE_UNKNOWN  PIGEON_TYPE_UNKNOWN

#define wrenGetVersionNumber     pigeonGetVersionNumber
#define wrenInitConfiguration    pigeonInitConfiguration
#define wrenNewVM                pigeonNewVM
#define wrenFreeVM               pigeonFreeVM
#define wrenCollectGarbage       pigeonCollectGarbage
#define wrenInterpret            pigeonInterpret
#define wrenMakeCallHandle       pigeonMakeCallHandle
#define wrenCall                 pigeonCall
#define wrenReleaseHandle        pigeonReleaseHandle
#define wrenGetSlotCount         pigeonGetSlotCount
#define wrenEnsureSlots          pigeonEnsureSlots
#define wrenGetSlotType          pigeonGetSlotType
#define wrenGetSlotBool          pigeonGetSlotBool
#define wrenGetSlotBytes         pigeonGetSlotBytes
#define wrenGetSlotDouble        pigeonGetSlotDouble
#define wrenGetSlotForeign       pigeonGetSlotForeign
#define wrenGetSlotString        pigeonGetSlotString
#define wrenGetSlotHandle        pigeonGetSlotHandle
#define wrenSetSlotBool          pigeonSetSlotBool
#define wrenSetSlotBytes         pigeonSetSlotBytes
#define wrenSetSlotDouble        pigeonSetSlotDouble
#define wrenSetSlotNewForeign    pigeonSetSlotNewForeign
#define wrenSetSlotNewList       pigeonSetSlotNewList
#define wrenSetSlotNewMap        pigeonSetSlotNewMap
#define wrenSetSlotNull          pigeonSetSlotNull
#define wrenSetSlotString        pigeonSetSlotString
#define wrenSetSlotHandle        pigeonSetSlotHandle
#define wrenGetListCount         pigeonGetListCount
#define wrenGetListElement       pigeonGetListElement
#define wrenSetListElement       pigeonSetListElement
#define wrenInsertInList         pigeonInsertInList
#define wrenGetMapCount          pigeonGetMapCount
#define wrenGetMapContainsKey    pigeonGetMapContainsKey
#define wrenGetMapValue          pigeonGetMapValue
#define wrenSetMapValue          pigeonSetMapValue
#define wrenRemoveMapValue       pigeonRemoveMapValue
#define wrenGetVariable          pigeonGetVariable
#define wrenHasVariable          pigeonHasVariable
#define wrenHasModule            pigeonHasModule
#define wrenAbortFiber           pigeonAbortFiber
#define wrenGetUserData          pigeonGetUserData
#define wrenSetUserData          pigeonSetUserData

#endif
