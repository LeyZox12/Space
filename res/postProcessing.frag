
uniform sampler2D screen;
uniform vec2 u_resolution;
uniform float pixelSize;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    uv = floor(gl_FragCoord.xy / pixelSize) * pixelSize / u_resolution.xy;
    gl_FragColor = texture2D(screen, uv);
}
