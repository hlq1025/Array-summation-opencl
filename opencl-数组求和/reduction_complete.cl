__kernel void reduction_complete(__global float* data, 
      __global float* partial_sums, __global float* sum) {

   int gid = get_global_id(0);
   int group_size = get_global_size(0);
   
   partial_sums[gid] = data[gid];
 
   barrier(CLK_GLOBAL_MEM_FENCE);
   int j=group_size;
   int i=group_size/2.0f+0.5;
   
	while(i>0) {
<<<<<<< HEAD
	if(gid==0) printf("%f\n",partial_sums[3001]);
=======
>>>>>>> f0166a17b41a87077a95b1f1a36aad20843522a0
	
      if(gid+i < j) {
	 
		
         partial_sums[gid] += partial_sums[gid + i];
		
<<<<<<< HEAD
		
      }
	 
     barrier(CLK_GLOBAL_MEM_FENCE);
=======
		 
      }
	 
      barrier(CLK_GLOBAL_MEM_FENCE);
>>>>>>> f0166a17b41a87077a95b1f1a36aad20843522a0
	
	 
	  j=i;
	  if(i!=1) i=i/2.0f+0.5;
<<<<<<< HEAD
	  else i=0;
=======
	  else i=i>>1;
>>>>>>> f0166a17b41a87077a95b1f1a36aad20843522a0
	 
   }

   if(gid == 0) {
      *sum = partial_sums[0];
	 
   }
}


