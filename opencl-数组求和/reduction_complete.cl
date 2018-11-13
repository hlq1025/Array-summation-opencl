__kernel void reduction_complete(__global float* data, 
      __local float* partial_sums, __global float* sum) {

   int gid = get_global_id(0);
   int group_size = get_global_size(0);
   
   partial_sums[gid] = data[gid];
   barrier(CLK_LOCAL_MEM_FENCE);

   for(int i = group_size/2; i>0; i >>= 1) {
      if(gid < i) {
         partial_sums[gid] += partial_sums[gid + i];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
   }

   if(gid == 0) {
      *sum = partial_sums[0];
   }
}


