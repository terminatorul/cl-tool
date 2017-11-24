
#ifndef FLOAT_TYPE
# ifdef cl_khr_fp64
#  define FLOAT_TPYE double
# else
#  define FLOAT_TYPE float
# endif
#endif

#define	FLOAT_FUNCTION(name_prefix, floating_type, name_suffix) name_prefix ## floating_type ## name_suffix
#define	FLOAT_FUNCTION_NAME(name_prefix, floating_type, name_suffix) FLOAT_FUNCTION(name_prefix, floating_type, name_suffix)

// random_fill_float_block(...)
// random_fill_double_block(...)
//
kernel void FLOAT_FUNCTION_NAME(random_fill_, float, _block)(global FLOAT_TYPE *matrix, ulong lines, ulong cols, FLOAT_TYPE minVal, FLOAT_TYPE maxVal)
{
    ulong2 const work_item_size = (ulong2)( (lines + get_global_size(0) - 1) / get_global_size(0), (cols + get_global_size(1) - 1) / get_global_size(1));
    unsigned const startLine = work_item_size.s0 * (get_global_id(0) - get_global_offset(0)) , startCol = work_item_size.s1 * (get_global_id(1) - get_global_offset(1));
    unsigned shift_register = (startLine ? startLine : (unsigned)copysign(maxVal, 1.0f)) * (startCol ? startCol : (unsigned)copysign(maxVal, 1.0f)) + (startLine > startCol ? 8 : 3);
    unsigned bit = 1;

    shift_register *= (startLine ? startLine : (unsigned)(maxVal - minVal) / 2) * (startCol ? startCol : (unsigned)(maxVal - minVal) / 2) + abs_diff(startCol, startLine) / 3;

    for (unsigned int i = startLine; i < min(lines, startLine + work_item_size.s0); i++)
	for (unsigned int j = startCol; j < min(cols, startCol + work_item_size.s1); j++)
	{
	    bit  = ((shift_register >> 0) ^ (shift_register >> 3) ^ (shift_register >> 5) ^ (shift_register >> 8) ) & 0x0001U;
	    shift_register =  (shift_register >> 1) | (shift_register << 31);

	    matrix[i * cols + j] = minVal + fmod((FLOAT_TYPE)(shift_register) / 100, maxVal - minVal);
	}
}

// multipy_float_matrix_block()
// multiply_double_matrix_block()
//
kernel void FLOAT_FUNCTION_NAME(multiply_, FLOAT_TYPE, _matrix_block)
    (
	global FLOAT_TYPE const *m, ulong m_lines, ulong m_cols,
	global FLOAT_TYPE const *n, ulong n_lines, ulong n_cols,
	global FLOAT_TYPE *result, ulong result_lines, ulong result_cols
    )
{
    if (m_lines == result_lines && m_cols == n_lines && n_cols == result_cols)
    {
	ulong2 const work_item_size = (ulong2)((result_lines + get_global_size(0) -1 ) / get_global_size(0), (result_cols + get_global_size(1) - 1) / get_global_size(1));
	unsigned const startLine = work_item_size.s0 * (get_global_id(0) - get_global_offset(0)), startCol = work_item_size.s1 * (get_global_id(1) - get_global_offset(1));

	    for (unsigned i = startLine; i < min(result_lines, startLine + work_item_size.s0); i++)
		for (unsigned j = startCol; j < min(result_cols, startCol + work_item_size.s1); j++)
		    for (unsigned k = 0; k < m_cols; k++)
			result[i * result_cols + j] += m[i * m_lines + k] * n[k * n_lines + j];
    }
}

/*
 * vi:ft=opencl:ts=8
 */
