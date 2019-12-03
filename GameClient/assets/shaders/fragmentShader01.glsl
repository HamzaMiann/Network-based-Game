#version 420

in vec4 fColour;	
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Get rid of this
uniform vec4 newColour;
// Make the shader use these:
uniform float newColourRed;
uniform float newColourGreen;
uniform float newColourBlue;

uniform vec3 lightPosition;


void main()
{
    //gl_FragColor = vec4(color, 1.0);
   //gl_FragColor = vec4(newColour, 1.0);
   
   float dist = distance( lightPosition, fVertWorldLocation.xyz );
   float linearAtten = 1.0f; 
   
   float atten = 1.0f / ( linearAtten * dist );
   
//   if (atten > .9f)
//	atten = 1.f;
//   else if (atten > .7f)
//	atten = .7f;
//   else if (atten > .5f)
//	atten = .5f;
//   else if (atten > .3f)
//	atten = .3f;
//   else if (atten > .1f)
//	atten = .1f;
//   else if (atten > .0f)
//	atten = 0.f;
	
	gl_FragColor = vec4(newColourRed,
						newColourGreen, 
	                    newColourBlue,
//						sin(vertWorld.z),
//						sin(vertWorld.x),
//						sin(vertWorld.y),
						1.0 );
	
	gl_FragColor *= atten;

}
