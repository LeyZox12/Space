precision highp float;

uniform vec2 u_resolution;
uniform sampler2D texture;
uniform vec2 pos;
uniform vec2 topLeft;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    gl_FragColor = vec4(uv.x, uv.y, 1.0, 1.0);
}