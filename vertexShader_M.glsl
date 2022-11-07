#version 330 core


layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1

uniform mat4 Projection;
uniform mat4 Model;
out vec4 ourColor; // output a color to the fragment shader

void main(){	

	gl_Position = Projection * Model * vec4(aPos.x,aPos.y,aPos.z,1.0);
	ourColor=aColor;
	
}

