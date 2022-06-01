/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>

int
print_result(int n, double e)
{
    if (n % 500000 == 0) {
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
        print_result(n, e);
    }

    return 0;
}
