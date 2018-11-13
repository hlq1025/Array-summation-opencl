__kernel void reduction_complete(__global float* data, 
      __local float* partial_sums, __global float* sum) {

   int gid = get_global_id(0);
   int group_size = get_global_size(0);
   
   partial_sums[gid] = data[gid];
   barrier(CLK_GLOBAL_MEM_FENCE);
   int j=group_size;
   int i=group_size/2+0.5;
   
	while(i>0) {
      if(gid < i) {
	  if(gid+i<j)
         partial_sums[gid] += partial_sums[gid + i];
      }
      barrier(CLK_GLOBAL_MEM_FENCE);
	  j=i;
	  if(i!=1) i=i/2.0f+0.5;
	  else i=i>>1;
	 
   }

   if(gid == 0) {
      *sum = partial_sums[0];
   }
}


