__kernel void product(
    __global int* input,
    __global int* output
) 
{
    int globalID = get_global_id(0);
	int localID = get_local_id(0);
	int localSize = get_local_size(0);
    __local int reduction[512];
	reduction[localID] = input[globalID];
	
	for(int offset = localSize / 2; offset > 0; offset /= 2) {
		barrier(CLK_LOCAL_MEM_FENCE);	// wait for all other work-items to finish previous iteration.
		if(localID < offset) {
			reduction[localID] *= reduction[localID + offset];
		}
	}
	
	if(localID == 0) {	// the root of the reduction subtree
        int workgroupID = globalID / localSize;
		output[workgroupID] = reduction[0];
	};
}