# ==============================================================
# File generated on Wed Aug 12 18:26:01 +0800 2020
# Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2018.3 (64-bit)
# SW Build 2405991 on Thu Dec  6 23:38:27 MST 2018
# IP Build 2404404 on Fri Dec  7 01:43:56 MST 2018
# Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
# ==============================================================
add_files -tb ../test.cpp -cflags { -Wno-unknown-pragmas}
add_files HLS_SURF/top.h
add_files HLS_SURF/top.cpp
add_files HLS_SURF/mType.h
add_files HLS_SURF/mSURF.h
add_files HLS_SURF/mSURF.cpp
set_part xc7z020clg400-2
create_clock -name default -period 10
config_compile -no_signed_zeros=0
config_compile -unsafe_math_optimizations=0
config_dataflow -default_channel=fifo
config_dataflow -fifo_depth=2
config_dataflow -scalar_fifo_depth=0
config_dataflow -start_fifo_depth=0
config_dataflow -strict_mode=warning
set_directive_pipeline SURF::integralImg/integralImg_col -II 2
set_directive_unroll SURF::integralImg/integralImg_channel 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_pipeline SURF::HessianDetector/HessianDetector_octave -II 2
set_directive_stream top 
set_directive_unroll SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_hessian 
set_directive_pipeline SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_col 
set_directive_unroll SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_initbufIndex 
set_directive_loop_tripcount SURF::findCharacteristicPoint/findCharacteristicPoint_layer -min 1 -max 3
set_directive_loop_tripcount SURF::findCharacteristicPoint/findCharacteristicPoint_r0 -min 1 -max 559
set_directive_loop_tripcount SURF::findCharacteristicPoint/findCharacteristicPoint_c0 -min 1 -max 858
set_directive_loop_tripcount SURF::findCharacteristicPoint/findCharacteristicPoint_c0 -min 1 -max 858
set_directive_pipeline SURF::createHessianBox 
set_directive_pipeline SURF::calcHaarPattern 
set_directive_pipeline SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_layer 
set_directive_pipeline SURF::findCharacteristicPoint/findCharacteristicPoint_c0 
set_directive_unroll SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_layer 
set_directive_dataflow SURF::HessianDetector 
set_directive_unroll SURF::findCharacteristicPoint/findCharacteristicPoint_layer 
set_directive_pipeline SURF::findCharacteristicPoint/findCharacteristicPoint_c0 
set_directive_pipeline SURF::calcHaarPattern_x_y 
set_directive_pipeline SURF::calcHaarPattern_xy 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::calcLayerDetAndTrace 
set_directive_array_partition SURF::findCharacteristicPoint 
set_directive_array_partition SURF::HessianDetector 
set_directive_array_partition SURF::HessianDetector 
