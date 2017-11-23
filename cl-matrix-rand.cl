
kernel void random_fill_float_block(global float *matrix, ulong lines, ulong cols, float minVal, float maxVal)
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

	    matrix[i * cols + j] = minVal + fmod((float)(shift_register) / 100, maxVal - minVal);
	}
}

kernel void multiply_float_matrix_block(global float const *m, ulong m_lines, ulong m_cols, global float const *n, ulong n_lines, ulong n_cols, global float *result, ulong result_lines, ulong result_cols)
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

#if 0 && defined(cl_khr_fp64)

__kernel void random_fill_double_block(__global double *matrix, ulong lines, ulong cols, double minVal, double maxVal)
{
    ulong2 const
	work_item_size = (ulong2)( (lines + get_global_size(0) - 1) / get_global_size(0), (cols + get_global_size(1) - 1) / get_global_size(1));
    unsigned
	startLine = work_item_size.s0 * (get_global_id(0) - get_global_offset(0)),
	startCol = work_item_size.s1 * (get_global_id(1) - get_global_offset(1));
    double
	absMaxVal = copysign((float)maxVal, 1);
    unsigned
	shift_register = (startLine ? startLine : (unsigned)absMaxVal + 2) * (startCol ? startCol : (unsigned)absMaxVal + 2) + (startLine > startCol ? 8 : 3),
	bit = 1;

    shift_register *= (startLine ? startLine : (unsigned)(maxVal - minVal) / 2) * (startCol ? startCol : (unsigned)(maxVal - minVal) / 2) + abs_diff(startCol, startLine) / 3;

    for (unsigned int i = startLine; i < min(lines, startLine + work_item_size.s0); i++)
	for (unsigned int j = startCol; j < min(cols, startCol + work_item_size.s1); j++)
	{
	    bit  = ((shift_register >> 0) ^ (shift_register >> 3) ^ (shift_register >> 5) ^ (shift_register >> 8) ) & 0x0001U;
	    shift_register =  (shift_register >> 1) | (shift_register << 31);

	    matrix[i * cols + j] = minVal + fmod((double)(shift_register) / 100.0, maxVal - minVal);
	}
}

#endif

/*
 * vi:ft=opencl
 */
