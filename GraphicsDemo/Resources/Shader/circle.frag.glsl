#version 430 core

in vec2 vUv;
in vec3 vPosition;

out vec4 FragColor;

float Circle(vec2 uv, float r, float blur)
{
    float d = length(uv);
    
    float c = smoothstep(r, r - blur, d);
    
    return c;
}

float Sun(vec2 uv, vec2 offset, float scale)
{
    const float radius = 0.4;
    const float petalDistance = 0.6;
    const float petalRadius = 0.12;
    const float blur = 0.01;
    const int petalCount = 10;
    
    float c = Circle(uv, radius * scale, blur);
    
    
    for( int i = 0; i < petalCount; ++i )
    {
        float angle = radians(float(360/petalCount*i));
        vec2 center = vec2(cos(angle), sin(angle)) * petalDistance;
        c += Circle(uv - center * scale, petalRadius * scale, blur);
    }
    
    return c;
}

vec3 sun(vec2 uv)
{
    uv  -= 0.5;  // -0.5 <> 0.5
    
    const float scale = 0.71;
    float c = Sun(uv, vec2(0.0), scale);
     
    return c;
}

void main(void)
{
    vec3 c = sun(vUv);

    if( c.r < 0.1 )
        discard;

    FragColor = vec4(c, 1.0);
}
