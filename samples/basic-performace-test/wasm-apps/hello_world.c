/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdio.h>
#include <stdlib.h>

int
main()
{
    int n = 1;
    double fac = 1.0;
    double e = 1.0;

    for (; n <= 200000000; n++) {
        fac *= n;
        e += 1.0 / fac;
    }

    return 0;
}
