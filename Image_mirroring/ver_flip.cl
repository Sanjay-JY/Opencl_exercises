__kernel void vertical(
    __global uchar* input,
    __global uchar* output,
    int width,
    int height
) 
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    output[y*width+x]=input[(((height-1)-y)*width)+x];
}