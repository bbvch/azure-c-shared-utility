// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef STRING_TOKENIZER_H
#define STRING_TOKENIZER_H

#include "azure_c_shared_utility/strings.h"

#ifdef __cplusplus
extern "C"
{
#else
#endif

typedef struct STRING_TOKEN_TAG* STRING_TOKENIZER_HANDLE;

extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create(STRING_HANDLE handle);
extern STRING_TOKENIZER_HANDLE STRING_TOKENIZER_create_from_char(const char* input);
extern int STRING_TOKENIZER_get_next_token(STRING_TOKENIZER_HANDLE t, STRING_HANDLE output, const char* delimiters);
extern void STRING_TOKENIZER_destroy(STRING_TOKENIZER_HANDLE t);

#ifdef __cplusplus
}
#else
#endif

#endif  /*STRING_TOKENIZER_H*/
