#version 420

//uniform mat4 MVP;
uniform mat4 matModel;					// Model or World 
uniform mat4 matModelInverTrans;		// Model or World 
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform
uniform float iTime;

// Texture
uniform sampler2D textSamp00;	// Main Texture
uniform sampler2D textSamp01;	// Specular Map
uniform sampler2D textSamp02;	// Normal Map
uniform sampler2D textSamp03;	// Ambient Occlusion map
uniform vec4 tex_0_3_ratio;		// x = 0, y = 1, z = 2, w = 3



in vec4 vColour;				// Was vec3
in vec4 vPosition;				// Was vec3
in vec4 vNormal;				// Vertex Normal
in vec4 vUVx2;					// 2 x Texture coord

//out vec3 color;
//out vec4 vertWorld;			// Location of the vertex in the world
out vec4 fColour;	
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;
out float fiTime;
out float fisWater;

float rand(float s, float r) { return mod(mod(s, r + iTime) * 112341, 1); }
float random (in vec2 st);
float noise (in vec2 st);

void main()
{
    vec4 vertPosition = vPosition;
    mat4 matMVP = matProj * matView * matModel;

	gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);

	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);

	//vec3 tex3_RGB = 1 - texture( textSamp03, fUVx2.st ).rgb;

	//gl_Position.y += length(tex3_RGB);
	//fVertWorldLocation.y += length(tex3_RGB);
	
	//mat4 matInv = inverse(transpose(matModel));

    fColour = vColour;
	fNormal = matModelInverTrans * vNormal;

	fUVx2 = vUVx2;

	fiTime = iTime;

}



float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}