
/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <pthread.h>
#include <unistd.h>
#include "wasm_export.h"
#include "bh_read_file.h"
#include "bh_getopt.h"

struct arg_struct {
    char *wasm_path;
    char *snap_path;
};

void
print_usage(void)
{
    fprintf(stdout, "Usage:\r\n");
    fprintf(stdout, "  [path of wasm file] [path of snapshot]\n");
}

int *
run_wasm(void *ptr)
{
    struct arg_struct *args = ptr;
    if (args->snap_path == NULL) {
        fprintf(stdout, "New start...\n");
    }
    else {
        fprintf(stdout, "Start from snapshot...\n");
        start_from_snapshot(args->snap_path);
    }

    static char global_heap_buf[512 * 1024];
    char *buffer, error_buf[128];

    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    uint32 buf_size, stack_size = 8092, heap_size = 8092;
    wasm_function_inst_t func = NULL;

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    // // Define an array of NativeSymbol for the APIs to be exported.
    // // Note: the array must be static defined since runtime
    // //            will keep it after registration
    // // For the function signature specifications, goto the link:
    // //
    // https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/doc/export_native_api.md

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    // 01. Initialize wasm runtime
    // Default config, use: wasm_runtime_init();
    // if (!wasm_runtime_full_init(&init_args)) {
    //     printf("Init runtime environment failed.\n");
    //     return -1;
    // }
    if (!wasm_runtime_init()) {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    // 02. Read WASM file into memory buffer
    buffer = bh_read_file_to_buffer(args->wasm_path, &buf_size);

    if (!buffer) {
        printf("Open wasm app file [%s] failed.\n", args->wasm_path);
        goto fail;
    }

    // 03. Parse WASM file from buffer and create a WASM module
    module = wasm_runtime_load(buffer, buf_size, error_buf, sizeof(error_buf));
    if (!module) {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    // 04. Create an instance of WASM module (WASM linear memory is ready)
    module_inst = wasm_runtime_instantiate(module, stack_size, heap_size,
                                           error_buf, sizeof(error_buf));

    if (!module_inst) {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    // 05. Create executation environment to execute WASM function
    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (!exec_env) {
        printf("Create wasm execution environment failed.\n");
        goto fail;
    }

    // 06. Lookup WASM function by name
    if (!(func = wasm_runtime_lookup_function(module_inst, "main", NULL))) {
        printf("The main wasm function is not found.\n");
        goto fail;
    }

    // // 07. Function call with parameters in an array of 32 bits elements and
    // size
    uint32 argv[4];
    // pass 2 elements for function arguments
    if (!wasm_runtime_call_wasm(exec_env, func, 2, argv)) {
        printf("call wasm function main failed. %s\n",
               wasm_runtime_get_exception(module_inst));
        goto fail;
    }

fail:
    if (exec_env)
        wasm_runtime_destroy_exec_env(exec_env);
    if (module_inst) {
        // if (wasm_buffer)
        //     wasm_runtime_module_free(module_inst, wasm_buffer);
        wasm_runtime_deinstantiate(module_inst);
    }
    if (module)
        wasm_runtime_unload(module);
    if (buffer)
        BH_FREE(buffer);
    wasm_runtime_destroy();
    return 0;
}

int *
run_phantom(void *ptr)
{
    printf("Phantom started\n");

    char *file_name0 = "snapshot0";
    char *file_name1 = "snapshot1";

    sleep(2);
    if (ptr) {
        printf("Stop...\n");
        stop();
    }
    else {
        printf("Take snapshot #0...\n");
        take_snapshot(file_name0);
        sleep(2);
        printf("Take snapshot #1 then stop...\n");
        take_snapshot_and_stop(file_name1);
    }

    printf("Phantom finished\n");
    return 0;
}

int
main(int argc, char *argv_main[])
{
    pthread_t wamr, phantom;
    int wamrret, phantomret;
    int opt;
    struct arg_struct args;
    args.snap_path = NULL;
    args.wasm_path = NULL;

    if (argc == 1) {
        print_usage();
        return 0;
    }

    args.wasm_path = argv_main[1];

    if (argc == 3) {
        args.snap_path = argv_main[2];
    }

    printf("WASM path: %s\n", args.wasm_path);

    if (args.snap_path != NULL) {
        printf("SNAP path: %s\n", args.snap_path);
    }

    init_action_primitives();
    wamrret = pthread_create(&wamr, NULL, run_wasm, (void *)&args);
    phantomret =
        pthread_create(&phantom, NULL, run_phantom, (void *)argv_main[2]);
    pthread_join(wamr, NULL);
    pthread_join(phantom, NULL);
    destroy_action_primitives();
    return 0;
}
