#version 120

attribute vec4 vPosition;
attribute vec4 vNormal;
varying vec4 color;

uniform int isShadow;
uniform mat4 ctm, model_view, projection;
uniform vec4 ambient_product, diffuse_product, specular_product, light_position;
uniform float shininess, attenuation_constant, attenuation_linear, attenuation_quadratic;

vec4 ambient, diffuse, specular, calcNorm;

void main()
{
	vec4 act_pos = ctm * vPosition;

	if(isShadow == 1){
		color = vec4(0, 0, 0, 1);
		gl_Position = ctm * vPosition;
		float x = light_position.x - light_position.y * (light_position.x - gl_Position.x) / (light_position.y - gl_Position.y);
		float y = 0.001;
		float z = light_position.z - light_position.y * (light_position.z - gl_Position.z) / (light_position.y - gl_Position.y);
		gl_Position = projection * model_view * vec4(x, y, z, 1.0);
	}
	else{
		ambient = ambient_product;

		vec4 N = normalize(vNormal);
		vec4 L_temp = (light_position - act_pos);
		vec4 L = normalize(L_temp);
		diffuse = max(dot(L,N), 0.0) * diffuse_product;

		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		vec4 V = normalize(eye_point - (act_pos));
		vec4 H = normalize(L + V);
		specular = pow(max(dot(N, H), 0.0), shininess) * specular_product;

		float distance = length(L_temp);
		float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) +
		(attenuation_quadratic * distance * distance));

		// Setting the actual variables to be passed to the renderer
		color = ambient + (attenuation * (diffuse + specular * 50));
		// color = ambient + specular * 10000;
		// color = normalize(vNormal);
		gl_Position = projection * model_view * act_pos;
	}
}
