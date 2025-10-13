
uniform vec2 u_resolution;
uniform sampler2D screen;
uniform float pixelSize;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    gl_FragColor = texture2D(screen, uv);
}