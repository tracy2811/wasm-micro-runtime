/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>

int
main()
{
    int n;
    double fac, e;

    for (int i = 0; i < 10; i++) {
        fac = 1.0;
        e = 1.0;
        for (n = 1; n <= 200000000; n++) {
            fac *= n;
            e += 1.0 / fac;
        }
    }

    return 0;
}
