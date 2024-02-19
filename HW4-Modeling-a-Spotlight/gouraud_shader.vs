#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

uniform mat4 model;
uniform mat4 view;

uniform mat4 projection;
uniform vec4 LightPosition;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

uniform float Spot_exponent;
uniform float Spot_cutoff;
uniform float Spot_direction;

out vec4 fColor;

void main()
{
  vec3 fN = VertexNormal;
  vec3 fE = VertexPosition;
  vec3 fL = LightPosition.xyz;

// Normalize the input lighting vectors

vec3 N = normalize(fN);
vec3 E = normalize(fE);
vec3 L = normalize(fL);

vec3 H = normalize(L+E);

vec4 ambient = AmbientProduct;
float Kd = max(dot(L,N), 0.0);
vec4 diffuse = Kd*DiffuseProduct;

float Ks = pow(max(dot(N, H), 0.0), Shininess);
vec4 specular = Ks*SpecularProduct;

fColor = ambient + diffuse + specular;

fColor.a = 1.0;

   
   gl_Position = projection*view*model * vec4(VertexPosition,1.0);
}


