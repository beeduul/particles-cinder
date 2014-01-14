#version 110

varying vec2 uv;
uniform vec4 color;
uniform sampler2D texture;

void
//main() {
//    float border = 0.1;
//    float radius = 0.5;
//    vec4 color0 = vec4(0.0, 0.0, 0.0, 0.0); // border
////    vec4 color0 = vec4(color.x, color.y, color.z, 0.0); // solid
//    vec4 color1 = color;
//    
//    vec2 m = uv - vec2(0.5, 0.5);
//    float dist = radius - sqrt(m.x * m.x + m.y * m.y);
//    
//    float t = 0.0;
//    if (dist > border)
//        t = 1.0;
//    else if (dist > 0.0)
//        t = dist / border;
//    
//    gl_FragColor = mix(color0, color1, t);
//}

main() {
    vec2 texCoord = gl_TexCoord[0].st;
    vec4 c = texture2D(texture, texCoord);
    gl_FragColor = c * color;
}