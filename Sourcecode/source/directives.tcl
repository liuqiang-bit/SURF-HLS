############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_pipeline -II 2 "SURF::integralImg/integralImg_col"
set_directive_unroll "SURF::integralImg/integralImg_channel"
set_directive_array_partition -type complete -dim 2 "SURF::calcLayerDetAndTrace" dx_s
set_directive_array_partition -type complete -dim 2 "SURF::calcLayerDetAndTrace" dy_s
set_directive_array_partition -type complete -dim 2 "SURF::calcLayerDetAndTrace" dxy_s
set_directive_pipeline -II 2 "SURF::HessianDetector/HessianDetector_octave"
set_directive_stream -depth 48000 -dim 1 "top" sumBuf
set_directive_unroll "SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_hessian"
set_directive_pipeline "SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_col"
set_directive_unroll "SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_initbufIndex"
set_directive_loop_tripcount -min 1 -max 3 "SURF::findCharacteristicPoint/findCharacteristicPoint_layer"
set_directive_loop_tripcount -min 1 -max 559 "SURF::findCharacteristicPoint/findCharacteristicPoint_r0"
set_directive_loop_tripcount -min 1 -max 858 "SURF::findCharacteristicPoint/findCharacteristicPoint_c0"
set_directive_loop_tripcount -min 1 -max 858 "SURF::findCharacteristicPoint/findCharacteristicPoint_c0"
set_directive_pipeline "SURF::createHessianBox"
set_directive_pipeline "SURF::calcHaarPattern"
set_directive_pipeline "SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_layer"
set_directive_pipeline "SURF::findCharacteristicPoint/findCharacteristicPoint_c0"
set_directive_unroll "SURF::calcLayerDetAndTrace/calcLayerDetAndTrace_layer"
set_directive_dataflow "SURF::HessianDetector"
set_directive_unroll "SURF::findCharacteristicPoint/findCharacteristicPoint_layer"
set_directive_pipeline "SURF::findCharacteristicPoint/findCharacteristicPoint_c0"
set_directive_pipeline "SURF::calcHaarPattern_x_y"
set_directive_pipeline "SURF::calcHaarPattern_xy"
set_directive_array_partition -type complete -dim 0 "SURF::calcLayerDetAndTrace" Dx
set_directive_array_partition -type complete -dim 0 "SURF::calcLayerDetAndTrace" Dy
set_directive_array_partition -type complete -dim 0 "SURF::calcLayerDetAndTrace" Dxy
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" sizes
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" sampleSteps
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" middleIndices
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" detRow
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" detCol
set_directive_array_partition -type complete -dim 0 "SURF::calcLayerDetAndTrace" sizes
set_directive_array_partition -type complete -dim 0 "SURF::calcLayerDetAndTrace" sampleSteps
set_directive_array_partition -type complete -dim 1 "SURF::calcLayerDetAndTrace" sumBuf
set_directive_array_partition -type complete -dim 0 "SURF::findCharacteristicPoint" bRow
set_directive_array_partition -type complete -dim 0 "SURF::HessianDetector" dets
set_directive_array_partition -type complete -dim 0 "SURF::HessianDetector" traces
