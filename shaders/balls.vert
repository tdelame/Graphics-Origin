#version 440
uniform mat4 mv;

in vec4 ball;
in vec3 color;

out float ball_radius;
out vec3  ball_color;

void main()
{
  gl_Position = mv * vec4( ball.xyz, 1.0 );
  ball_color  = color; 
  ball_radius = ball.w;
}