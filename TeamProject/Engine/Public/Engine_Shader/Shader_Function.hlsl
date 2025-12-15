#ifndef __SHADER_FUNCTION_HLSL__
#define __SHADER_FUNCTION_HLSL__

float2 CalculateFrameIndex(uint Col, uint Row, uint FrameIndex)
{
    float2 FrameSize = float2(1.f / Col, 1.f / Row);
    
    int iFrameX = FrameIndex % Col;
    int iFrameY = FrameIndex / Col;
    
    float2 Frame = float2(iFrameX, iFrameY) * FrameSize;
  
    return Frame;
}

#endif