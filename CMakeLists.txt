# Copyright (c) 2021 Martin Jäger / Libre Solar
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)

project(frag_decoder)

include_directories(.)

add_executable(frag_decoder
    main_stdin.c
    #main.c
    frag.c
    bitmap.c
)

# for math.h functions
target_link_libraries(frag_decoder m)
