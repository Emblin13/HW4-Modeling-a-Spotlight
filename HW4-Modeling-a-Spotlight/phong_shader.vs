#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 fColor;

// Normalized direction towards light source in eye coords.
uniform vec4 LightPosition;
uniform vec3 DiffuseProduct;            // Diffuse reflectivity
uniform vec3 AmbientProduct;            // Ambient reflectivity
uniform vec3 SpecularProduct;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor
uniform float Spot_exponent;
uniform float Spot_cutoff;
uniform float Spot_direction;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 ads( vec4 position, vec3 norm )
{
    vec3 s, v;
    if( LightPosition.w == 0.0 ){
      s = normalize(vec3(LightPosition));
      v = normalize(vec3(position));
    }
    else{
      s = normalize( vec3(LightPosition - position) );
      v = normalize(vec3(-position));
    }

   
    vec3 h = normalize(s + v);

    return
         AmbientProduct +  DiffuseProduct* max( dot(s, norm), 0.0 ) + SpecularProduct * pow( max( dot(norm, h), 0.0 ), Shininess );
}

void main()
{
    vec3 eyeNorm;
    vec4 eyePosition;

     if( LightPosition.w == 0.0 ){
         eyeNorm = normalize(VertexNormal);
         eyePosition = vec4(VertexPosition,1.0);
     }
     else{
         eyeNorm = normalize(mat3(view*model)*VertexNormal);
         eyePosition = view*model*vec4(VertexPosition,1.0); 
     }
    

    // Evaluate the lighting equation
    fColor = ads( eyePosition, eyeNorm );

    gl_Position = projection*view*model*vec4(VertexPosition,1.0);
}
