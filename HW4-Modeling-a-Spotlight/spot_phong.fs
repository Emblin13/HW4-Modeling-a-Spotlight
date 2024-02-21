#version 400

in vec4 Position;
in vec3 Normal;

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

uniform unsigned int On;

layout( location = 0 ) out vec4 FragColor;

vec3 adsWithSpotlight( )
{
    vec3 s = normalize( vec3( Spot.position) - Position.xyz );
    vec3 spotDir = normalize( Spot.direction);
    float angle = acos( dot(-s, spotDir) );
    float cutoff = radians( Spot.cutoff);
   
    vec3 ambient = vec3(AmbientProduct);

    if(On == 1) {
		return AmbientProduct + DiffuseProduct + SpecularProduct;
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
    FragColor = min(vec4(adsWithSpotlight(), 1.0), vec4(1.0));
}

