uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec2 u_resolution;
uniform vec2 resolution1;
uniform vec2 resolution2;
uniform vec2 pos1;
uniform vec2 pos2;
uniform float rot1;
uniform vec2 scale1;
uniform vec2 scale2;
uniform float pixelSize;

#define black vec4(0.0, 0.0, 0.0, 0.0)

bool isInBounds(vec2 uv)
{
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

void main()
{
    vec2 size1 = resolution1 * scale1;
    vec2 p1 = pos1;
    vec2 p2 = pos2;
    p1.y = u_resolution.y - pos1.y;
    p2.y = u_resolution.y - pos2.y;
    mat2 rot = mat2(cos(rot1), -sin(rot1), sin(rot1), cos(rot1));
    vec2 offset = p1 / u_resolution;
    vec2 uv = gl_FragCoord.xy / u_resolution;
    vec2 uv1 = floor(((gl_FragCoord.xy-p1-size1/2.0)*rot+size1/2.0) / pixelSize) * pixelSize / resolution1 /scale1;
    vec2 uv2 = floor((gl_FragCoord.xy-p2 + resolution2*scale2 / 2.0) / pixelSize) * pixelSize / resolution2/scale2;
    uv.y = 1.0 - uv.y;
    uv1.y = 1.0-uv1.y;
    uv2.y = 1.0-uv2.y;

    vec4 c = vec4(0.0);
    vec4 c1 = vec4(0.0);
    if(isInBounds(uv1))c1= texture2D(texture1,uv1);
    vec4 c2 = texture2D(texture2,uv2);
    if(isInBounds(uv2))c2= texture2D(texture2,uv2);
    
    if(c1 != black && c2 != black) c = vec4(1.0, 0.0, 1.0, 1.0);
    gl_FragColor = c;
}