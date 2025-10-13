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

#define black vec4(0.0, 0.0, 0.0, 0.0)

bool isInBounds(vec2 uv)
{
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

void main()
{
    pos1.y = u_resolution.y - pos1.y;
    pos2.y = u_resolution.y - pos2.y;
    mat2 rot = mat2(cos(rot1), -sin(rot1), sin(rot1), cos(rot1));
    vec2 offset = pos1 / u_resolution;
    vec2 uv = gl_FragCoord.xy / u_resolution;
    vec2 uv1 = (gl_FragCoord.xy-pos1)*rot / resolution1 /scale1;
    vec2 uv2 = (gl_FragCoord.xy-pos2) / resolution2/scale2;
    uv.y = 1.0 - uv.y;
    uv1.y = 1.0-uv1.y;
    uv2.y = 1.0-uv2.y;

    vec4 c = vec4(0.0);
    vec4 c1 = vec4(0.0);
    if(isInBounds(uv1))c1= texture2D(texture1,uv1);
    vec4 c2 = texture2D(texture2,uv2);
    if(isInBounds(uv2))c2= texture2D(texture2,uv2);
    
    if(c1 != black && c2 != black) c = vec4(1.0, 0.0, 0.9176, 1.0);
    else if(c1 != black) c = c1;
    else if(c2 != black) c = c2;
    gl_FragColor = c;
}