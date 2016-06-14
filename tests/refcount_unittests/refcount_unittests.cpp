// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"

#include "azure_c_shared_utility/lock.h"

#include "some_refcount_impl.h"



static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

#define GBALLOC_H

extern "C" int gballoc_init(void);
extern "C" void gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void gballoc_free(void* ptr);

namespace BASEIMPLEMENTATION
{
    /*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

};

static size_t currentmalloc_call;
static size_t whenShallmalloc_fail;

TYPED_MOCK_CLASS(CRefCountMocks, CGlobalMock)
{
public:

    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2;
    currentmalloc_call++;
    if (whenShallmalloc_fail>0)
    {
        if (currentmalloc_call == whenShallmalloc_fail)
        {
            result2 = NULL;
        }
        else
        {
            result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
        }
    }
    else
    {
        result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
    }
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_1(CRefCountMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CRefCountMocks, , void, gballoc_free, void*, ptr);

BEGIN_TEST_SUITE(refcount_unittests)

    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        MicroMockDestroyMutex(g_testByTest);
        DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
        currentmalloc_call = 0;
        whenShallmalloc_fail = 0;
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

    TEST_FUNCTION(refcount_create_returns_non_NULL)
    {
        ///arrange
        POS_HANDLE p;
        CRefCountMocks mocks;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);

        ///act
        p = Pos_Create(4);

        ///assert
        ASSERT_IS_NOT_NULL(p);
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        Pos_Destroy(p);
    }

    TEST_FUNCTION(refcount_DEC_REF_after_create_says_we_should_free)
    {
        ///arrange
        POS_HANDLE p;
        CRefCountMocks mocks;
        STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
            .IgnoreArgument(1);
        p = Pos_Create(4);
        mocks.ResetAllCalls();

        ///act
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        Pos_Destroy(p);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
    }

    TEST_FUNCTION(refcount_INC_REF_and_DEC_REF_after_create_says_we_should_not_free)
    {
        ///arrange
        pos* p, *clone_of_p;
        CRefCountMocks mocks;
        p = Pos_Create(2);
        clone_of_p = Pos_Clone(p);
        mocks.ResetAllCalls();

        ///act
        Pos_Destroy(p);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
        Pos_Destroy(p);
    }

    TEST_FUNCTION(refcount_after_clone_it_takes_2_destroys_to_free)
    {
        ///arrange
        pos* p, *clone_of_p;
        CRefCountMocks mocks;
        p = Pos_Create(2);
        clone_of_p = Pos_Clone(p);
        Pos_Destroy(p);
        mocks.ResetAllCalls();

        ///act
        STRICT_EXPECTED_CALL(mocks, gballoc_free(IGNORED_PTR_ARG))
            .IgnoreArgument(1);
        Pos_Destroy(clone_of_p);

        ///assert
        mocks.AssertActualAndExpectedCalls();

        //cleanup
    }

END_TEST_SUITE(refcount_unittests)

