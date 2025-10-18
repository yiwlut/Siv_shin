//-----------------------------------------------
// standard_2d.vert (템플릿)
//-----------------------------------------------
#version 410

// macOS 호환성을 위한 gl_PerVertex 재선언
//out gl_PerVertex
//{
//    vec4 gl_Position;
//};

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 VertexUV;
layout(location = 2) in vec4 VertexColor;

layout(location = 0) out vec4 Color;
layout(location = 1) out vec2 UV;

layout(std140) uniform VSConstants2D
{
    mat4 g_transform;
};

void main()
{
    gl_Position = g_transform * vec4(VertexPosition, 0.0, 1.0);
    Color = VertexColor;
    UV = VertexUV;
}
