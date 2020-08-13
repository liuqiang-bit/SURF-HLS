############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project HLS_SURF
set_top top
add_files HLS_SURF/mSURF.cpp
add_files HLS_SURF/mSURF.h
add_files HLS_SURF/mType.h
add_files HLS_SURF/top.cpp
add_files HLS_SURF/top.h
add_files -tb HLS_SURF/test.cpp -cflags "-Wno-unknown-pragmas"
open_solution "solution7"
set_part {xc7z020clg400-2}
create_clock -period 10 -name default
config_compile -no_signed_zeros=0 -unsafe_math_optimizations=0
config_dataflow -default_channel fifo -fifo_depth 2 -scalar_fifo_depth 0 -start_fifo_depth 0 -strict_mode warning
source "./HLS_SURF/solution7/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
