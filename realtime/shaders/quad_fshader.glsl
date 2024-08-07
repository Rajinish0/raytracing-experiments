#version 330 core

uniform sampler2D texture_screen;
in vec2 texCoord;
out vec4 fragcol;

const float offset = 1.0 / 300.0;
void main() {

	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), // top-left
		vec2(0.0f, offset), // top-center
		vec2(offset, offset), // top-right
		vec2(-offset, 0.0f),   // center-left
		vec2(0.0f, 0.0f),   // center-center
		vec2(offset, 0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2(0.0f, -offset), // bottom-center
		vec2(offset, -offset)  // bottom-right
	);

	/*float kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);*/

	float kernel[9] = float[](
		1.0/9.0, 1.0/9.0, 1.0/9.0,
		1.0/9.0, 1.0/9.0, 1.0/9.0,
		1.0/9.0, 1.0/9.0, 1.0/9.0
		);

	//edge detection (horizontal edges -- left to right)
	//float kernel[9] = float[] (
	//	-1.0, 0.0, 1.0,
	//	-1.0, 0.0, 1.0,
	//	-1.0, 0.0, 1.0
	//);

	vec3 samples[9];
	for (int i = 0; i < 9; i++) {
		vec2 fin = texCoord.st + offsets[i];
		fin.x = max(0, min(fin.x, 1.0));
		fin.y = max(0, min(fin.y, 1.0));
		samples[i] = vec3(texture(texture_screen, fin ));
	}

	vec3 col = vec3(0.0);
	for (int i = 0; i < 9; ++i) {
		col += kernel[i] * samples[i];
	}
	//fragcol = vec4(1.0, 0.0, 0.0, 1.0);
	fragcol = texture(texture_screen, texCoord);

	/*
	* color inversion
	*/
	//fragcol = vec4(vec3(1.0 - fragcol), 1.0);

	/*
	* cnvt to gray scale
	*/
	//float avg = (fragcol.r + fragcol.g + fragcol.b) / 3.0;
	//fragcol = vec4(vec3(avg), 1.0);

	/*
	* weighted gray scale, i forgot the name of this but this is available on wikipedia
	*/

	/*
	float avg = 0.2126 * fragcol.r + 0.7152 * fragcol.g + 0.0722 * fragcol.b;
	fragcol = vec4(vec3(avg), 1.0);
	*/

	/*
	* conv of a kernel on a image
	*/
	fragcol = vec4(col, 1.0);
}