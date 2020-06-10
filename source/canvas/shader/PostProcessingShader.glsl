// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// VERTEX SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef VERTEX_SHADER

// Vertex shader input
layout (location = 0) in vec2 position;


// Vertex shader output
out vec2 TexCoords;


// Vertex shader main
void main(void){
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    TexCoords = 0.5f * (position + vec2(1.0f));
}

#endif /* VERTEX_SHADER */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FRAGMENT SHADER
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef FRAGMENT_SHADER

// Fragment shader input
in vec2 TexCoords;


// Fragment shader output
layout (location = 0) out vec3 FragColor;


// Uniforms / Constants
uniform sampler2D textureGUI;
uniform vec3 colorScale;


// Fragment shader main
void main(void){
    vec4 guiColor = texture(textureGUI, TexCoords);

    // Final fragment color
    FragColor = guiColor.rgb * colorScale;
}

#endif /* FRAGMENT_SHADER */

