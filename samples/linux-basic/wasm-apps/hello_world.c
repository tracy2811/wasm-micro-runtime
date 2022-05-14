/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>

float
generate_float(int iteration, double seed1, float seed2)
{
    float ret;

    printf("calling into WASM function: %s\n", __FUNCTION__);

    for (int i = 0; i < iteration; i++) {
        ret += 1.0f / seed1 + seed2;
    }

    return ret;
}

int
print_result(int n, double e)
{
    if (n % 100000 == 0) {
        printf("N=%d: e=%lf\n", n, e);
    }
    return 0;
}

int
main()
{
    int n = 1;
    double fac = 1.0;
    double e = 1.0;

    for (; n > 0; n++) {
        fac *= n;
        e += 1.0 / fac;
        // print_result(n, e);
        if (n % 100000 == 0) {
            printf("N=%d: e=%lf\n", n, e);
        }
    }

    return 0;
}
