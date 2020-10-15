constant float h = 0.001f;		// time step

constant float L1 = 0.43f;		// first rod length in the double pendulum
constant float m1 = 1.23f;		// first mass in the double pendulum

constant float L2 = 0.48f;		// second rod length in the double pendulum
constant float m2 = 1.49f;		// second mass in the double pendulum
constant float g  = 9.81f;

float4 stateDerivative(float4 private const *state)
{
	private float denom = 2 * m1 + m2 - m2 * cos(2 * (*state).x - 2 * (*state).y);

	return (float4)
		(
			(*state).z,
			(*state).w,

			(
				-g * (2 * m1 * m2) * sin((*state).x)
					-
				m2 * g * sin((*state).x - 2 * (*state).y)
					-
				2 * sin((*state).x - (*state).y) * m2 * ( (*state).w * (*state).w * L2 + (*state).z * (*state).z * L1 * cos((*state).x - (*state).y))
			)
				/
			(L1 * denom),

			2 * sin((*state).x - (*state).y) * ((*state).z * (*state).z * L1 * (m1 + m2) + g * (m1 + m2) * cos((*state).x) + (*state).w * (*state).w * L2 * m2 * cos((*state).x - (*state).y))
				/
			(L2 * denom)
		);
}

void runRungeKuttaStep(float4 private *state)
{
	float4 derivative_a = stateDerivative(state);

	float4 updated_state = *state + h / 2.0f * derivative_a;
	float4 derivative_b = stateDerivative(&updated_state);

	updated_state = *state + h / 2.0f * derivative_b;
	float4 derivative_c = stateDerivative(&updated_state);

	updated_state = *state + h * derivative_c;
	float4 derivative_d = stateDerivative(&updated_state);

	derivative_a += 2 * (derivative_b + derivative_c) + derivative_d;
	derivative_a *= h / 6.0f;

	*state += derivative_a;

	(*state).x = fmod((*state).x, 2 * M_PI_F);
	(*state).y = fmod((*state).y, 2 * M_PI_F);
}

kernel void doublePendulumSimulation(global char *result_matrix, unsigned long stepCount)
{
	float4 state;

	// Read work item index as a fraction of group size from -1.0 to +1.0
	//
	state.x = 2.0f * get_global_id(0) / (get_global_size(0) - 1U) - 1.0f;

	// Translate work item index to fraction between 1/3 and 9/10 of pi
	state.x *= (9.0f / 10.0f - 1.0f / 3.0f);
	state.x += 1.0f / 3.0f;
	state.x *= M_PI_F;

	state.y = state.x;
	state.z = 0.0f;
	state.w = 0.0f;

	unsigned long i = 0;

	for (i = 0; i < stepCount; i++)
		runRungeKuttaStep(&state);

	result_matrix[get_global_id(0)] = state.x * 10;
	// result_matrix[get_global_id(0)] = state.y;
}
