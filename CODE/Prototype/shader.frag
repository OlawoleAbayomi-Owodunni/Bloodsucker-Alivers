//CREDIT: CUSTOM OUTLINE SHADER BY KAI DEVI

#version 130
uniform sampler2D texture;

void main()
{
    vec2 offsets[8] = vec2[](
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2(1.0, -1.0),
        vec2(-1.0, 0.0),                      vec2(1.0, 0.0),
        vec2(-1.0, 1.0),  vec2(0.0, 1.0),  vec2(1.0, 1.0)
    );

    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    if (pixel.a < 0.01)
    {
        bool isOpaque = false;
        for (int i = 0; i < 8; i++)
        {
            vec2 offsetTexCoord = gl_TexCoord[0].xy + offsets[i] / textureSize(texture,0);
            vec4 neighborColor = texture2D(texture, offsetTexCoord);
            if (neighborColor.a > 0.99)
            {
                isOpaque = true;
                break;
            }


        }

        if (isOpaque)
            gl_FragColor = vec4(0.0,0.0,0.0,1.0);
        else
            discard;
    }
    else
    {
        // multiply it by the color
        gl_FragColor = gl_Color * pixel;
    }


}