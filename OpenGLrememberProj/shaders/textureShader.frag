uniform sampler2D tex;

varying vec2 texCoord;

void main(void)
{
	vec3 tex_vec = texture2D(tex,texCoord).rgb;
    gl_FragColor = vec4(tex_vec,1.0); //натягиваем текстуру
    gl_FragColor-=vec4(0.2,0.2,0.2,0.0);  //чучуть ее затемняем
}
