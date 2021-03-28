/*
MIT License

Copyright (c) 2021 Hirrolot

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// The official repository: <https://github.com/Hirrolot/result99>.

#ifndef RESULT99_H
#define RESULT99_H

#include <datatype99.h>

#ifndef RESULT99_NO_ALIASES

#define IoError      IoError99
#define Result       Result99
#define isResultOk   isResultOk99
#define isResultErr  isResultErr99
#define Err          Err99
#define tryResult    tryResult99
#define tryResultMap tryResultMap99

#endif // RESULT99_NO_ALIASES

typedef int IoError99;

#define Result99(name, T, _E) datatype99(name##Result, (name##Ok, T), (name##E, _E))

#define isResultOk99(result)  ((int)(result).tag == 0)
#define isResultErr99(result) ((int)(result).tag == 1)

#define Err99(result_ty, failure_expr) result_ty##E(result_ty##E_##failure_expr)

#define tryResult99(result, result_ty, ok_var, ...)                                                \
    tryResultMap99(result, result_ty, ok_var, (e, result_ty##E(*e)), __VA_ARGS__)

// clang-format off

#define tryResultMap99(result, result_ty, ok_var, on_failure, ...)                                 \
    match99(result) {                                                                              \
        of99(result_ty##Ok, ok_var) __VA_ARGS__                                                    \
        of99(result_ty##E, ML99_TUPLE_GET(0)(on_failure)) return ML99_TUPLE_GET(1)(on_failure);    \
    }                                                                                              \
                                                                                                   \
    do {                                                                                           \
    } while (0)
// clang-format on

#endif // RESULT99_H
