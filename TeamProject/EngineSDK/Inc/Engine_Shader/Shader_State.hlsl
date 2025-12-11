#ifndef __SHADER_STATE_HLSL__
#define __SHADER_STATE_HLSL__

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CW
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = true;
};

RasterizerState RS_NoCull
{
    FillMode = Solid;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullFront
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullBack
{
    FillMode = Solid;
    CullMode = BACK;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_ReadOnly
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha; /* µ¤À» »ö»ó*/
    BlendOp = Add;
};

BlendState BS_Premultiplied
{
    BlendEnable[0] = true;
    SrcBlend = One; 
    DestBlend = Inv_Src_Alpha; 
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};
BlendState BS_Additive
{
    BlendEnable[0] = true;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};
BlendState BS_Blend_CloudShadow
{
    BlendEnable[0] = TRUE;

    // color: src * srcAlpha + dest * (1 - srcAlpha)
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;

    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ONE;
    BlendOpAlpha = ADD;

    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BS_UI_AlphaBlend
{
    BlendEnable[0] = true;
    SrcBlend = one;
    DestBlend = Inv_Src_Alpha; /* µ¤À» »ö»ó*/
    BlendOp = Add;
};

SamplerState DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
SamplerState LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};
SamplerState PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
SamplerState PointLinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
#endif // __SHADER_STATE_HLSL__