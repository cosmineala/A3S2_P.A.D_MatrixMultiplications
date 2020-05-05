
__kernel void matix_multiplication(__global const int *M1, __global const int *M2, __global int *M3)
{
   int i = get_global_id(0); 
   int j = get_global_id(1);
   int size = get_global_size(0);

   int x, temp = 0;

   for (x = 0; x < size; x++)   {

        temp += M1[i * size + x] * M2[x * size + j];
   
   }

   M3[i * size + j] = temp;
}
