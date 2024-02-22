#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


struct SpotLightInfo {
    vec4 position;   // Position in eye coords
    vec4 intensity;
    vec3 direction;  // Direction of the spotlight in eye coords.
    float exponent;  // Angular attenuation exponent
    float cutoff;    // Cutoff angle (between 0 and 90)
};
uniform SpotLightInfo Spot;

uniform vec4 DiffuseProduct;            // Diffuse reflectivity
uniform vec4 AmbientProduct;            // Ambient reflectivity
uniform vec4 SpecularProduct;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

uniform int On;

out vec4 FragColor;
vec3 adsWithSpotlight( vec4 Position, vec3 Normal )
{
    vec3 s = normalize( vec3( Spot.position) - Position.xyz );
    vec3 spotDir = normalize( Spot.direction);
    float angle = acos( dot(-s, spotDir) );
    float cutoff = radians( Spot.cutoff);
   
    vec3 ambient = vec3(AmbientProduct);
    vec3 diffuse = vec3(DiffuseProduct);
    vec3 specular = vec3(SpecularProduct);

    if(On == 1) {
		return ambient + diffuse + specular;
	}
    else if( angle < cutoff ) {
        float spotFactor = pow( dot(-s, spotDir), Spot.exponent );
        vec3 v = normalize(vec3(-Position.xyz));
        vec3 h = normalize( v + s );

        return
            ambient +
             spotFactor*(
              vec3(DiffuseProduct) * max( dot(s, Normal), 0.0 ) +
              vec3(SpecularProduct) * pow( max( dot(h,Normal), 0.0 ), Shininess )
           );
    } else {
        return ambient;
    }


}

void main() {
    vec4 Pos = view*model*VertexPosition;

    vec3 Nor = normalize(mat3(view*model)*VertexNormal);
   
    FragColor = min(vec4(adsWithSpotlight(Pos, Nor), 1.0), vec4(1.0));

    gl_Position = projection*view*model*VertexPosition;
}
