__kernel void hello(__global int* matAA, __global int* matBB, __global int* matCC)
{
	unsigned int i = get_global_id(0);
	matCC[i] = matAA[i] + matBB[i];
}