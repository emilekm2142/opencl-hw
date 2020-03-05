const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
CLK_ADDRESS_CLAMP_TO_EDGE |
CLK_FILTER_NEAREST;

__kernel void grayscale(__read_only  image2d_t src,
    __write_only image2d_t dst)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    uint4 color;

    color = read_imageui(src, sampler, (int2)(x, y));
    uint gray = (color.x + color.y + color.z) / 3;
    write_imageui(dst, (int2)(x, y), (uint4)(gray, gray, gray, 0));
}