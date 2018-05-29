#version 420 core

uniform mat4 PROJECTION;
uniform mat4 VIEW; 
uniform mat4 MODEL;
uniform float TIME;

in vec3 POSITION;
in vec4 COLOR;       // NEW - GLSL will use a Vector4 for this; 
in vec2 UV;         

out vec2 passUV; 
out vec4 passColor;  // NEW - to use it in the pixel stage, we must pass it.
out float passTime;

void main() 
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * VIEW * MODEL * local_pos; 

   passColor = COLOR; // pass it on. 
   passUV = UV; 
   passTime = TIME;
   gl_Position = clip_pos; 
}