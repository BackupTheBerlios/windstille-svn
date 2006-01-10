// -*- c++ -*-

// Shockwave shader, good look version

uniform sampler2D background_tex;
uniform sampler2D noise_tex;
uniform float     radius;

void main()
{
  float x_dist = gl_TexCoord[0].x - 0.5;
  float y_dist = gl_TexCoord[0].y - 0.5;
  // Distance from center of the explosion
  float dist   = sqrt(x_dist*x_dist + y_dist*y_dist);

  if (dist > radius || dist < radius - 164.0/512.0) 
    {
      discard;
    } 
  else 
    {
      vec2 pos = vec2(float(gl_FragCoord.x)/800.0, 
                      float(gl_FragCoord.y)/600.0) * vec2(800.0/1024.0, 600.0/1024.0);

      vec4 uv_col  = texture2D(noise_tex, gl_TexCoord[0].xy*1.5);
      float h = 1.0 - clamp((radius - dist)*512.0/164.0, 0.0, 1.0);
      h *= h;
      vec4 col = texture2D(background_tex, pos + (uv_col.rb * uv_col.g) * h * 0.3);

      float highlight = 1.0 - clamp((radius - dist)*512.0/32.0, 0.0, 1.0);

      col += vec4(h * uv_col.r + highlight*0.9,
                  h * uv_col.r + highlight*0.9,
                  h*1.5 * uv_col.r + highlight, 
                  1);

      gl_FragColor = col;
    }
}

/* EOF */
