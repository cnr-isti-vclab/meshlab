uniform int texSize;
varying vec2 texCoord;

void main(void)
{
/*
   gl_Position = vec4( gl_Vertex.xy, 0.0, 1.0 );
   gl_Position = sign( gl_Position );
    
   //Normalize texCoord between 0.0 - 1.0 then multiply it by texSize
   texCoord = (vec2( gl_Position.x, - gl_Position.y ) + vec2( 1.0 ) ) / vec2( 2.0 );
   texCoord *= vec2(texSize);
/**/
   gl_Position = gl_Vertex;
}