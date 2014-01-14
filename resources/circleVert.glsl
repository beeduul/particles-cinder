//void main()
//{
//    gl_Position = ftransform();
////    gl_TexCoord[0] = gl_MultiTexCoord0;
//}

#version 110

varying vec2 uv;

void
main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;

    uv = vec2(gl_MultiTexCoord0);
}
