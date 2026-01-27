#ifndef __SHADER_STATE_HLSL__
#define __SHADER_STATE_HLSL__

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Command
{
    FillMode = Solid;
    CullMode = NONE;
    FrontCounterClockwise = false;
    DepthBias = -50.f; 
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

RasterizerState RS_Shadow
{
    FillMode = Solid;
    CullMode = Back; 
    FrontCounterClockwise = false;
    DepthBias = 1000;
    DepthBiasClamp = 0.01f;
    SlopeScaledDepthBias = 2.0f; 
    DepthClipEnable = true;
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

DepthStencilState DSS_MotionStencil
{
    DepthEnable = true;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
    
    StencilEnable = true;
    FrontFaceStencilFunc = EQUAL;
    FrontFaceStencilPass = INCR;
    BackFaceStencilFunc = EQUAL;
    BackFaceStencilPass = INCR;
    
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;
};

DepthStencilState DSS_WriteStencil
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;

    StencilEnable = TRUE;
    StencilReadMask = 0xff;
    StencilWriteMask = 0xff;

    FrontFaceStencilFunc = Always;
    FrontFaceStencilPass = Replace;
    BackFaceStencilFunc = Always;
    BackFaceStencilPass = Replace;
};

DepthStencilState DSS_OutlineStencil
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;

    StencilEnable = TRUE;
    StencilReadMask = 0xff;
    StencilWriteMask = 0x00;

    FrontFaceStencilFunc = Not_Equal;
    FrontFaceStencilPass = Keep;
    BackFaceStencilFunc = Not_Equal;
    BackFaceStencilPass = Keep;

};

DepthStencilState DSS_UIWriteStencil
{
    DepthEnable          = FALSE;
    DepthWriteMask       = ZERO;
                         
    StencilEnable        = TRUE;
    StencilReadMask      = 0xff;
    StencilWriteMask     = 0xff;

    FrontFaceStencilFunc = Always;
    FrontFaceStencilPass = Replace;
    BackFaceStencilFunc  = Always;
    BackFaceStencilPass  = Replace;
};

DepthStencilState DSS_UIStencilTest
{
    DepthEnable          = FALSE;
    DepthWriteMask       = ZERO;
    DepthFunc            = LESS_EQUAL;
                         
    StencilEnable        = TRUE;
    StencilReadMask      = 0xff;
    StencilWriteMask     = 0x00;

    FrontFaceStencilFunc = Equal;
    FrontFaceStencilPass = Keep;
    BackFaceStencilFunc  = Equal;
    BackFaceStencilPass  = Keep;
};

BlendState BS_ColorWriteOff
{
    BlendEnable[0]           = false;
    RenderTargetWriteMask[0] = 0x00;
};

DepthStencilState DSS_ReadOnly
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Command
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;
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

    SrcBlendAlpha = One;
    DestBlendAlpha = One;
    BlendOpAlpha = Add;
};

BlendState BS_SrcAdditive
{
    BlendEnable[0] = true;
    SrcBlend = Src_Alpha;
    DestBlend = One;
    BlendOpAlpha = Add;
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

BlendState BS_OITCompositeCombined
{
    BlendEnable[0] = true;

    SrcBlend = One;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = Add;
};

BlendState BS_OITAccmulation
{
    /* Diffuse Effect */
    BlendEnable[0] = true;
    SrcBlend[0] = One;
    DestBlend[0] = One;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;

    /* Bloom Effect */
    BlendEnable[1] = true;
    SrcBlend[1] = One;
    DestBlend[1] = One;
    BlendOp[1] = Add;
    SrcBlendAlpha[1] = One;
    DestBlendAlpha[1] = One;
    BlendOpAlpha[1] = Add;

    /* Bloom Info */
    BlendEnable[2] = true;
    SrcBlend[2] = One;
    DestBlend[2] = One;
    BlendOp[2] = Add;
    SrcBlendAlpha[2] = One;
    DestBlendAlpha[2] = One;
    BlendOpAlpha[2] = Add;

    /* Revealage */
    BlendEnable[3] = true;
    SrcBlend[3] = Zero;
    DestBlend[3] = Inv_Src_Alpha;
    BlendOp[3] = Add;
    SrcBlendAlpha[3] = Zero;
    DestBlendAlpha[3] = Inv_Src_Alpha;
    BlendOpAlpha[3] = Add;

    /* Distortion */
    BlendEnable[4] = true;
    SrcBlend[4] = One;
    DestBlend[4] = One;
    BlendOp[4] = Add;
    SrcBlendAlpha[4] = One;
    DestBlendAlpha[4] = One;
    BlendOpAlpha[4] = Add;
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
SamplerState LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
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