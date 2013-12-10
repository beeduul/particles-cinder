#version 110
varying float depth;
uniform float depthThresholdLo;
uniform float depthThresholdHi;

void main()
{
    if( depth < depthThresholdLo ) discard;
    if (depth > depthThresholdHi ) discard;

	gl_FragColor.rgb	= vec3( depth );
	gl_FragColor.a		= 1.0;
}





