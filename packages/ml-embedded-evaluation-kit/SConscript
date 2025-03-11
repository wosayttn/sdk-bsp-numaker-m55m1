from building import *
import rtconfig
import os

MODEL_FLAGS=' -DACTIVATION_BUF_SZ=0x00082000 '

# M55M1/TC8263
CMSIS_FLAGS=' -DETHOSU55=1 -DARM_NPU -DETHOSU_ARCH=u55  -DCMSIS_NN=1 -DCMSIS_DSP=1 -DETHOSU_MACS=256 -DETHOSU=1 '
NPU_FLAGS = CMSIS_FLAGS + ' -DETHOS_U_BASE_ADDR=0x40003000 -DETHOS_U_IRQN=13 -DETHOS_U_SEC_ENABLED=1 -DETHOS_U_PRIV_ENABLED=1 '

MLEVK_FLAGS = NPU_FLAGS + MODEL_FLAGS

LOCAL_CFLAGS = ' '
LOCAL_CXXFLAGS = ' -std=c++14 -Wno-psabi -DARM_MATH_LOOPUNROLL -D__ARM_FEATURE_DSP=1 -DARM_MODEL_USE_PMU_COUNTERS=1 '

LOCAL_CFLAGS = LOCAL_CFLAGS + MLEVK_FLAGS
LOCAL_CXXFLAGS = LOCAL_CXXFLAGS + MLEVK_FLAGS

group = []
cwd = GetCurrentDir() # get current dir path

def check_h_hpp_exists(path):
    file_dirs = os.listdir(path)
    for file_dir in file_dirs:
        if os.path.splitext(file_dir)[1] in ['.h', '.hpp']:
            return True
    return False

mlevk_cwd = cwd
dependencies_cwd = os.path.join(mlevk_cwd, 'dependencies')

dependencies_npu_drv_cwd = os.path.join(dependencies_cwd, 'core-driver')
npu_drv_INCS = [
    dependencies_npu_drv_cwd + '/include',
]
npu_drv_SRCS = Split("""
dependencies/core-driver/src/ethosu_device_u55_u65.c
dependencies/core-driver/src/ethosu_driver.c
dependencies/core-driver/src/ethosu_pmu.c
""")
group = group + DefineGroup('mlevk_npu_drv', npu_drv_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = npu_drv_INCS, LOCAL_CFLAGS = LOCAL_CFLAGS, LOCAL_CXXFLAGS = LOCAL_CXXFLAGS)

dependencies_cmsis_nn_cwd = os.path.join(dependencies_cwd, 'cmsis-nn')
cmsis_nn_INCS = [
    dependencies_cmsis_nn_cwd,
    dependencies_cmsis_nn_cwd + '/Include',
]
cmsis_nn_SRCS = Split("""
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_mul_s16_batch_offset.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_add_s8.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_mul_s16.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_mul_s8.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_mul_acc_s16.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_add_s16.c
dependencies/cmsis-nn/Source/BasicMathFunctions/arm_elementwise_mul_s16_s8.c
dependencies/cmsis-nn/Source/ActivationFunctions/arm_nn_activation_s16.c
dependencies/cmsis-nn/Source/ActivationFunctions/arm_relu_q15.c
dependencies/cmsis-nn/Source/ActivationFunctions/arm_relu_q7.c
dependencies/cmsis-nn/Source/ActivationFunctions/arm_relu6_s8.c
dependencies/cmsis-nn/Source/SVDFunctions/arm_svdf_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/SVDFunctions/arm_svdf_state_s16_s8.c
dependencies/cmsis-nn/Source/SVDFunctions/arm_svdf_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_s4.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_batch_matmul_s16.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_s16.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_wrapper_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_get_buffer_sizes_s16.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_vector_sum_s8_s64.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_batch_matmul_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_vector_sum_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_per_channel_s8.c
dependencies/cmsis-nn/Source/FullyConnectedFunctions/arm_fully_connected_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_depthwise_conv_s8_core.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1x1_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_get_buffer_sizes_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_transpose_conv_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_row_offset_s8_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1_x_n_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_get_buffer_sizes_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_wrapper_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1x1_s8_fast.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_wrapper_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_get_buffer_sizes_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_3x3_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1x1_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1x1_s4_fast.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_s4_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_s8_opt.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_s8_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_s4_opt.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_transpose_conv_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_1_x_n_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_fast_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_nn_mat_mult_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_wrapper_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_even_s4.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_convolve_wrapper_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_wrapper_s16.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_wrapper_s8.c
dependencies/cmsis-nn/Source/ConvolutionFunctions/arm_depthwise_conv_get_buffer_sizes_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mult_nt_interleaved_t_even_s4.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_per_ch_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mul_result_acc_s8_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_padded_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nntables.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_lstm_step_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s4.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_depthwise_conv_nt_t_s4.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s4.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s16_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_svdf_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mult_nt_t_s8_s32.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mul_core_1x_s4.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_s8_to_s16_unordered_with_offset.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_vec_mat_mult_t_s8.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_lstm_calculate_gate_s8_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_lstm_step_s16.c
dependencies/cmsis-nn/Source/NNSupportFunctions/arm_nn_mat_mul_core_4x_s8.c
dependencies/cmsis-nn/Source/ReshapeFunctions/arm_reshape_s8.c
dependencies/cmsis-nn/Source/ConcatenationFunctions/arm_concatenation_s8_y.c
dependencies/cmsis-nn/Source/ConcatenationFunctions/arm_concatenation_s8_x.c
dependencies/cmsis-nn/Source/ConcatenationFunctions/arm_concatenation_s8_z.c
dependencies/cmsis-nn/Source/ConcatenationFunctions/arm_concatenation_s8_w.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_avgpool_s8.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_max_pool_s16.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_max_pool_s8.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_avgpool_get_buffer_sizes_s8.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_avgpool_s16.c
dependencies/cmsis-nn/Source/PoolingFunctions/arm_avgpool_get_buffer_sizes_s16.c
dependencies/cmsis-nn/Source/LSTMFunctions/arm_lstm_unidirectional_s16.c
dependencies/cmsis-nn/Source/LSTMFunctions/arm_lstm_unidirectional_s8.c
dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_s8.c
dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_nn_softmax_common_s8.c
dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_u8.c
dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_s8_s16.c
dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_s16.c
""")
group = group + DefineGroup('mlevk_cmsis_nn', cmsis_nn_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = cmsis_nn_INCS, LOCAL_CFLAGS = LOCAL_CFLAGS, LOCAL_CXXFLAGS = LOCAL_CXXFLAGS)

dependencies_cmsis_dsp_cwd = os.path.join(dependencies_cwd, 'cmsis-dsp')
cmsis_dsp_INCS = [
    dependencies_cmsis_dsp_cwd,
    dependencies_cmsis_dsp_cwd + '/PrivateInclude',
    dependencies_cmsis_dsp_cwd + '/Include',
]
cmsis_dsp_SRCS = Split("""
dependencies/cmsis-dsp/Source/TransformFunctions/arm_bitreversal_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_f64.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_init_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix8_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_init_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_bitreversal.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_init_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_bitreversal2.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_init_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_init_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_init_f64.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_init_f64.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_mfcc_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix4_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix8_f16.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_f64.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_rfft_fast_init_f32.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_init_q15.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_cfft_radix2_init_q31.c
dependencies/cmsis-dsp/Source/TransformFunctions/arm_dct4_q15.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_conj_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mag_q31.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mult_real_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mag_q15.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mag_f16.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mult_real_q15.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c
dependencies/cmsis-dsp/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c
dependencies/cmsis-dsp/Source/MatrixFunctions/arm_mat_vec_mult_q15.c
dependencies/cmsis-dsp/Source/MatrixFunctions/arm_mat_vec_mult_f16.c
dependencies/cmsis-dsp/Source/MatrixFunctions/arm_mat_vec_mult_f32.c
dependencies/cmsis-dsp/Source/MatrixFunctions/arm_mat_vec_mult_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vlog_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_sin_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_cos_f32.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_atan2_f16.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_atan2_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_cos_q15.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vlog_f16.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_divide_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_sqrt_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vexp_f16.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_atan2_f32.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vlog_f64.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vexp_f64.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vlog_q15.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_sin_q15.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vexp_f32.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_sqrt_q15.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vlog_f32.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_divide_q15.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_vinverse_f16.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_sin_f32.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_cos_q31.c
dependencies/cmsis-dsp/Source/FastMathFunctions/arm_atan2_q15.c
dependencies/cmsis-dsp/Source/CommonTables/arm_const_structs_f16.c
dependencies/cmsis-dsp/Source/CommonTables/arm_common_tables_f16.c
dependencies/cmsis-dsp/Source/CommonTables/arm_mve_tables_f16.c
dependencies/cmsis-dsp/Source/CommonTables/arm_common_tables.c
dependencies/cmsis-dsp/Source/CommonTables/arm_const_structs.c
dependencies/cmsis-dsp/Source/CommonTables/arm_mve_tables.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_shift_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_clip_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_clip_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_xor_u32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_not_u8.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_not_u16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_or_u32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_shift_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_and_u16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_xor_u8.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_clip_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_not_u32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_or_u16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_shift_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_clip_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_f64.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_xor_u16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_clip_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_sub_q7.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_or_u8.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_q15.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_mult_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_abs_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_scale_f32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_negate_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_offset_q31.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_and_u8.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_and_u32.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_dot_prod_f16.c
dependencies/cmsis-dsp/Source/BasicMathFunctions/arm_add_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_var_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_kullback_leibler_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_entropy_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_rms_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_accumulate_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_std_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_var_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_logsumexp_dot_prod_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_entropy_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_std_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_rms_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_kullback_leibler_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_var_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_logsumexp_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_accumulate_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_std_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_rms_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_logsumexp_dot_prod_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_var_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_std_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_entropy_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_power_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_logsumexp_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_no_idx_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mse_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_accumulate_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_rms_q31.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_std_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_q15.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_f32.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmin_no_idx_q7.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_absmax_no_idx_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_mean_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_kullback_leibler_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_max_no_idx_f64.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_var_f16.c
dependencies/cmsis-dsp/Source/StatisticsFunctions/arm_min_q15.c
""")
group = group + DefineGroup('mlevk_cmsis_dsp', cmsis_dsp_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = cmsis_dsp_INCS, LOCAL_CFLAGS = LOCAL_CFLAGS, LOCAL_CXXFLAGS = LOCAL_CXXFLAGS)

dependencies_tflm_cwd = os.path.join(dependencies_cwd, 'tensorflow')
tflm_INCS = [
    dependencies_tflm_cwd,
    dependencies_tflm_cwd + '/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include',
    dependencies_tflm_cwd + '/tensorflow/lite/micro/tools/make/downloads/gemmlowp',
    dependencies_tflm_cwd + '/tensorflow/lite/micro/tools/make/downloads/ruy',
    dependencies_tflm_cwd + '/tensorflow/lite/micro/tools/make/downloads/kissfft',
]

tflm_SRCS = Split("""
dependencies/tensorflow/tensorflow/compiler/mlir/lite/schema/schema_utils.cc
dependencies/tensorflow/tensorflow/lite/array.cc
dependencies/tensorflow/tensorflow/lite/core/c/common.cc
dependencies/tensorflow/tensorflow/lite/core/api/tensor_utils.cc
dependencies/tensorflow/tensorflow/lite/core/api/error_reporter.cc
dependencies/tensorflow/tensorflow/lite/core/api/flatbuffer_conversions.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/tensor_ctypes.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/tensor_utils.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/runtime_shape.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/common.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/reference/comparisons.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/reference/portable_tensor_utils.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/quantization_util.cc
dependencies/tensorflow/tensorflow/lite/kernels/internal/portable_tensor_utils.cc
dependencies/tensorflow/tensorflow/lite/kernels/kernel_util.cc
dependencies/tensorflow/tensorflow/lite/micro/tflite_bridge/flatbuffer_conversions_bridge.cc
dependencies/tensorflow/tensorflow/lite/micro/tflite_bridge/micro_error_reporter.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_interpreter_graph.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_context.cc
dependencies/tensorflow/tensorflow/lite/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/arena_allocator/recording_single_arena_buffer_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/arena_allocator/single_arena_buffer_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/arena_allocator/persistent_arena_buffer_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/fake_micro_context.cc
dependencies/tensorflow/tensorflow/lite/micro/system_setup.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_profiler.cc
dependencies/tensorflow/tensorflow/lite/micro/test_helper_custom_ops.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_log.cc
dependencies/tensorflow/tensorflow/lite/micro/memory_planner/linear_memory_planner.cc
dependencies/tensorflow/tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc
dependencies/tensorflow/tensorflow/lite/micro/memory_planner/non_persistent_buffer_planner_shim.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_utils.cc
dependencies/tensorflow/tensorflow/lite/micro/mock_micro_graph.cc
dependencies/tensorflow/tensorflow/lite/micro/test_helpers.cc
dependencies/tensorflow/tensorflow/lite/micro/recording_micro_allocator.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_interpreter_context.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_interpreter.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_resource_variable.cc
dependencies/tensorflow/tensorflow/lite/micro/flatbuffer_utils.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_allocation_info.cc
dependencies/tensorflow/tensorflow/lite/micro/micro_op_resolver.cc
dependencies/tensorflow/tensorflow/lite/micro/cortex_m_generic/micro_time.cc
dependencies/tensorflow/tensorflow/lite/micro/cortex_m_generic/debug_log.cc
dependencies/tensorflow/tensorflow/lite/micro/memory_helpers.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/pooling.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/mul.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/fully_connected.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/softmax.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/depthwise_conv.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/conv.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/add.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/svdf.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/transpose_conv.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cmsis_nn/unidirectional_sequence_lstm.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/div.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/split.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/expand_dims.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/gather.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/pad.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/logical_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/quantize_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/space_to_depth.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cumsum.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/circular_buffer.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/conv_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/var_handle.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/elementwise.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/hard_swish_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/cast.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/arg_min_max.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/activations_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/unpack.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/log_softmax.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/logical.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/select.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/lstm_eval_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/sub_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/leaky_relu.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/l2_pool_2d.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/svdf_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/sub.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/kernel_runner.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/concatenation.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/floor_mod.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/comparisons.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/assign_variable.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/activations.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/tanh.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/fill.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/broadcast_to.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/elu.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/floor.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/strided_slice.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/logistic.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/add_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/logistic_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/transpose.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/exp.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/reshape.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/reduce.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/pack.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/if.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/add_n.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/neg.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/ethos_u/ethosu.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/lstm_eval.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/reshape_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/gather_nd.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/reduce_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/strided_slice_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/squeeze.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/leaky_relu_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/dequantize_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/maximum_minimum.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/circular_buffer_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/call_once.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/floor_div.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/squared_difference.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/hard_swish.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/mirror_pad.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/shape.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/micro_tensor_utils.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/batch_to_space_nd.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/while.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/batch_matmul.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/round.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/fully_connected_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/space_to_batch_nd.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/read_variable.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/quantize.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/resize_bilinear.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/kernel_util.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/split_v.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/broadcast_args.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/l2norm.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/mul_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/slice.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/softmax_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/prelu_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/detection_postprocess.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/pooling_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/embedding_lookup.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/dequantize.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/ceil.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/depth_to_space.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/depthwise_conv_common.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/prelu.cc
dependencies/tensorflow/tensorflow/lite/micro/kernels/zeros_like.cc
dependencies/tensorflow/signal/src/msb_64.cc
dependencies/tensorflow/signal/src/rfft_int32.cc
dependencies/tensorflow/signal/src/filter_bank_spectral_subtraction.cc
dependencies/tensorflow/signal/src/max_abs.cc
dependencies/tensorflow/signal/src/irfft_int32.cc
dependencies/tensorflow/signal/src/pcan_argc_fixed.cc
dependencies/tensorflow/signal/src/filter_bank_log.cc
dependencies/tensorflow/signal/src/circular_buffer.cc
dependencies/tensorflow/signal/src/square_root_64.cc
dependencies/tensorflow/signal/src/overlap_add.cc
dependencies/tensorflow/signal/src/filter_bank_square_root.cc
dependencies/tensorflow/signal/src/window.cc
dependencies/tensorflow/signal/src/irfft_int16.cc
dependencies/tensorflow/signal/src/energy.cc
dependencies/tensorflow/signal/src/rfft_float.cc
dependencies/tensorflow/signal/src/square_root_32.cc
dependencies/tensorflow/signal/src/log.cc
dependencies/tensorflow/signal/src/rfft_int16.cc
dependencies/tensorflow/signal/src/irfft_float.cc
dependencies/tensorflow/signal/src/kiss_fft_wrappers/kiss_fft_int16.cc
dependencies/tensorflow/signal/src/kiss_fft_wrappers/kiss_fft_float.cc
dependencies/tensorflow/signal/src/kiss_fft_wrappers/kiss_fft_int32.cc
dependencies/tensorflow/signal/src/msb_32.cc
dependencies/tensorflow/signal/src/filter_bank.cc
dependencies/tensorflow/signal/src/fft_auto_scale.cc
dependencies/tensorflow/signal/micro/kernels/fft_auto_scale_common.cc
dependencies/tensorflow/signal/micro/kernels/filter_bank_square_root.cc
dependencies/tensorflow/signal/micro/kernels/overlap_add.cc
dependencies/tensorflow/signal/micro/kernels/filter_bank_square_root_common.cc
dependencies/tensorflow/signal/micro/kernels/irfft.cc
dependencies/tensorflow/signal/micro/kernels/rfft.cc
dependencies/tensorflow/signal/micro/kernels/filter_bank.cc
dependencies/tensorflow/signal/micro/kernels/window.cc
dependencies/tensorflow/signal/micro/kernels/energy.cc
dependencies/tensorflow/signal/micro/kernels/filter_bank_log.cc
dependencies/tensorflow/signal/micro/kernels/fft_auto_scale_kernel.cc
dependencies/tensorflow/signal/micro/kernels/pcan.cc
dependencies/tensorflow/signal/micro/kernels/framer.cc
dependencies/tensorflow/signal/micro/kernels/delay.cc
dependencies/tensorflow/signal/micro/kernels/stacker.cc
dependencies/tensorflow/signal/micro/kernels/filter_bank_spectral_subtraction.cc
""")
TF_CXXFLAGS = CMSIS_FLAGS + ' -DNDEBUG=1 -DFLATBUFFERS_LOCALE_INDEPENDENT=0 '
group = group + DefineGroup('mlevk_tflm', tflm_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = tflm_INCS, LOCAL_CXXFLAGS = TF_CXXFLAGS)

mlevk_source_cwd = os.path.join(cwd, 'source')
mlevk_INCS = [
    mlevk_source_cwd + '/application/main/include',
    mlevk_source_cwd + '/application/api/common/include',
    mlevk_source_cwd + '/hal/include/',
    mlevk_source_cwd + '/hal/source/platform/rtthread/include',
    mlevk_source_cwd + '/hal/source/components/platform_pmu/include',
    mlevk_source_cwd + '/hal/source/components/lcd/source/rtthread',
    mlevk_source_cwd + '/hal/source/components/lcd/include',
    mlevk_source_cwd + '/hal/source/components/uvc/include',
    mlevk_source_cwd + '/hal/source/components/camera/include',
    mlevk_source_cwd + '/hal/source/components/audio/include',
    mlevk_source_cwd + '/hal/source/components/ext_file/include',
    mlevk_source_cwd + '/hal/source/components/memheap_helper/include',
    mlevk_source_cwd + '/hal/source/components/poster/include',
    mlevk_source_cwd + '/hal/source/components/stdout/include',
    mlevk_source_cwd + '/hal/source/components/npu/include',
    mlevk_source_cwd + '/log/include/',
    mlevk_source_cwd + '/math/include/',
    mlevk_source_cwd + '/profiler/include/',
]

mlevk_SRCS = Split("""
source/application/main/Main.cc
source/application/main/UseCaseCommonUtils.cc
source/application/api/common/source/Model.cc
source/application/api/common/source/ImageUtils.cc
source/application/api/common/source/Mfcc.cc
source/application/api/common/source/Classifier.cc
source/application/api/common/source/TensorFlowLiteMicro.cc
source/hal/source/components/stdout/source/user_input.c
source/hal/source/components/lcd/source/rtthread/lcd_helper.c
source/hal/source/components/uvc/source/uvc_img.c
source/hal/source/components/uvc/source/descriptors.c
source/hal/source/components/camera/source/camera_img.c
source/hal/source/components/audio/source/audio_helper.c
source/hal/source/components/ext_file/source/ext_file.c
source/hal/source/components/memheap_helper/source/memheap_helper.c
source/hal/source/components/poster/source/poster_helper.c
source/hal/source/components/poster/source/mqtt_pub.c
source/hal/source/components/npu/ethosu_impl_rtt.c
source/hal/source/components/npu/ethosu_npu_init.c
source/hal/source/components/npu/ethosu_profiler.c
source/hal/source/hal.c
source/hal/source/hal_pmu.c
source/hal/source/platform/rtthread/source/platform_drivers.c
source/hal/source/platform/rtthread/source/timer_native.c
source/math/PlatformMath.cc
source/profiler/Profiler.cc
""")
group = group + DefineGroup('mlevk_core', mlevk_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = mlevk_INCS, LOCAL_CFLAGS = LOCAL_CFLAGS, LOCAL_CXXFLAGS = LOCAL_CXXFLAGS)

#USE_CASE COMMON
mlevk_uc_cwd = os.path.join(cwd, 'source', 'use_case')
mlevk_uc_res_cwd = os.path.join(cwd, 'ethos-u55-256-gnu_generated')
mlevk_uc_api_cwd = os.path.join(cwd, 'source/application/api/use_case')
mlevk_uc_INCS = []
mlevk_uc_SRCS = []
MODEL_FLAGS = ''
mlevk_uc_name = ''

#MLEVK_UC_OBJECT_DETECTION
if GetDepend('MLEVK_UC_OBJECT_DETECTION'):
    mlevk_uc_name = 'object_detection'

elif GetDepend('MLEVK_UC_VWW'):
    mlevk_uc_name = 'vww'

elif GetDepend('MLEVK_UC_IMAGE_CLASS'):
    mlevk_uc_name = 'img_class'

elif GetDepend('MLEVK_UC_AD'):
    mlevk_uc_name = 'ad'

elif GetDepend('MLEVK_UC_INFERENCE_RUNNER'):
    mlevk_uc_name = 'inference_runner'

elif GetDepend('MLEVK_UC_ASR'):
    mlevk_uc_name = 'asr'

elif GetDepend('MLEVK_UC_KWS'):
    mlevk_uc_name = 'kws'

elif GetDepend('MLEVK_UC_KWS_ASR'):
    mlevk_uc_name = 'kws_asr'
    mlevk_uc_INCS = mlevk_uc_INCS + [
        os.path.join(mlevk_uc_api_cwd, 'asr', 'include'),
        os.path.join(mlevk_uc_api_cwd, 'kws', 'include'),
    ]
    mlevk_uc_SRCS = mlevk_uc_SRCS + Glob(os.path.join(mlevk_uc_api_cwd, 'kws', 'src','*.cc'))
    mlevk_uc_SRCS = mlevk_uc_SRCS + Glob(os.path.join(mlevk_uc_api_cwd, 'asr', 'src','*.cc'))

elif GetDepend('MLEVK_UC_NOISE_REDUCTION'):
    mlevk_uc_name = 'noise_reduction'

mlevk_uc_INCS = mlevk_uc_INCS + [
    os.path.join(mlevk_uc_cwd, mlevk_uc_name, 'include'),
    os.path.join(mlevk_uc_api_cwd, mlevk_uc_name, 'include'),
    os.path.join(mlevk_uc_res_cwd, mlevk_uc_name, 'include'),
]
mlevk_uc_SRCS = mlevk_uc_SRCS + Glob(os.path.join(mlevk_uc_cwd, mlevk_uc_name, 'src','*.cc'))
mlevk_uc_SRCS = mlevk_uc_SRCS + Glob(os.path.join(mlevk_uc_res_cwd, mlevk_uc_name, 'src','*.cc'))
mlevk_uc_SRCS = mlevk_uc_SRCS + Glob(os.path.join(mlevk_uc_api_cwd, mlevk_uc_name, 'src','*.cc'))

group = group + DefineGroup('mlevk_usecase', mlevk_uc_SRCS, depend = ['NU_PKG_USING_MLEVK'], CPPPATH = mlevk_uc_INCS, LOCAL_CFLAGS = LOCAL_CFLAGS, LOCAL_CXXFLAGS = LOCAL_CXXFLAGS)

list = os.listdir(cwd)
for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        group = group + SConscript(os.path.join(d, 'SConscript'))

Return('group')
