#version 420


#define TAU 6.28318530718
#define MAX_ITER 5

in vec4 ourColor;
out vec4 FragColor;  

uniform float time;
uniform vec2 res;
uniform int sceltaFS;

float hash(vec2 p)  // replace this by something better
{
    p  = 50.0*fract( p*0.3183099 + vec2(0.71,0.113));
    return -1.0+2.0*fract( p.x*p.y*(p.x+p.y) );
}

float map(float value, float input_start, float input_end, float output_start, float output_end) {
	return output_start + ((output_end - output_start) / (input_end - input_start)) * (value - input_start);
}

float noise( in vec2 p )
{
    vec2 i = floor( p );
    vec2 f = fract( p );
	
	vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

float noise(float x, float amplitude) {
	float speed = 0.0005;
	float n = sin(x*8.0 + time*speed) * 0.05 +
		sin(x*27.3 + time * speed) * 0.005 + 
		sin(time * speed) * 0.01;
	return n * amplitude;
}

// 0: normale, 1: sole, 2: acqua, 3: whirl, 4: whirl2, 5: sole e onde
void main( void )
{
    if(sceltaFS == 0) {
        FragColor = ourColor;
    }
    if(sceltaFS == 1) {
	    vec2 uv = gl_FragCoord.xy / res.y;
    
        // animation
        vec2 sunVec;
        float t = time;
    
        //sunVec = vec2((0.8+0.5 * sin(t)),( 0.44 + 0.4 * cos( 2.0 * t)));
        sunVec = vec2(0.2,0.2);
     	
   
        //Mie mask
        float sun = max(1.0 - (1.0 + 2.0 * (sunVec.y-0.1) + 1.0 * uv.y) * length(uv - sunVec),0.0)
            + 0.3 * pow(1.0-uv.y,12.0) * (1.6-sunVec.y);
	
        //the sauce
        FragColor = vec4(mix(vec3(0.3984,0.5117,0.7305), vec3(0.7031,0.4687,0.1055), sun)
                  * ((0.5 + 1.0 * pow(sunVec.y - 0.1,0.4)) * (1.5-uv.y) + pow(sun, 5.2)
                  * sunVec.y * (5.0 + 15.0 * sunVec.y)),1.0);
    
    }

    if(sceltaFS == 2) {
		float letime = time * .001;
		vec2 uv = gl_FragCoord.xy / res.xy;
    

		vec2 p = mod(uv*TAU, TAU)-250.0;

		vec2 i = vec2(p);
		float c = 1.0;
		float inten = .005;

		for (int n = 0; n < MAX_ITER; n++) {
			float t = letime * (1.0 - (3.5 / float(n+1)));
			i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
			c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
		}
		c /= float(MAX_ITER);
		c = 1.17-pow(c, 1.4);
		vec3 colour = vec3(pow(abs(c), 8.0));
		vec3 rgbColour = vec3(0.1, 0.2, 1);
		colour = clamp(colour + rgbColour, 0.0, 1.0);
	
	
		if((colour.r >= colour.g && colour.g >= colour.b) || (colour.r >= colour.b && colour.g >= colour.b)){
			FragColor = vec4(colour, vec3(max(colour.r*0.1, max(colour.g*0.1, colour.b))));
		}else if(colour.r >= colour.g && colour.b >= colour.g || (colour.r >= colour.b && colour.b >= colour.g)){
			FragColor = vec4(colour, vec3(max(colour.r*0.1, max(colour.b*0.1, colour.g))));
		}else if((colour.g >= colour.r && colour.r >= colour.b) || (colour.g >= colour.b && colour.r >= colour.b)){
			FragColor = vec4(colour, vec3(max(colour.g*0.1, max(colour.r*0.1, colour.b))));
		}else if((colour.g >= colour.r && colour.b >= colour.r) || (colour.g >= colour.b && colour.b >= colour.r)){
			FragColor = vec4(colour, vec3(max(colour.g*0.1, max(colour.b*0.1, colour.r))));
		}else if((colour.b >= colour.g && colour.g >= colour.r) || (colour.b >= colour.r && colour.g >= colour.r)){
			FragColor = vec4(colour, vec3(max(colour.b*0.1, max(colour.g*0.1, colour.r))));
		}else if((colour.b >= colour.g && colour.r >= colour.g) || (colour.b >= colour.r && colour.r >= colour.g)){
			FragColor = vec4(colour, vec3(max(colour.b*0.1, max(colour.r*0.1, colour.g))));
		}
    }

	if(sceltaFS == 3) {

		float speed = 5000;
		vec2 position = 4.*( gl_FragCoord.xy / res.xy );


		for(int n=1; n<100; n++){
			float i = float(n);
			position += vec2(.7/i*sin(i*position.y+time/speed+.9*i)+.8, .4/i*sin(i*position.x+time/speed+.5*(i+10))+1.6);
		}
	
		float colorr = .5*sin(position.x ) + .9;
		float colorg = .3*sin(position.y ) + .3;

		if(colorg > 0.5) colorg = 0.5;

		FragColor = vec4( vec3( 0 ,colorg,colorr), 0.7);
	}

	if(sceltaFS == 4) {
		float fluid_speed = 5000;
		float color_intensity = 0.2;
		vec2 p=(2.0*gl_FragCoord.xy-res)/max(res.x,res.y);
		for(int i=1;i<20;i++) {
			vec2 newp=p;
			newp.x+=0.5/float(i)*sin(float(i)*p.y+time/fluid_speed+0.9*float(i))+100.0;
			newp.y+=0.5/float(i)*sin(float(i)*p.x+time/fluid_speed+0.5*float(i+10))-100.0;
			p=newp;
		}

		vec3 col=vec3(color_intensity*sin(3.0*p.x)+color_intensity + 0.3,color_intensity*sin(3.0*p.y)+color_intensity + 0.2,sin(p.x+p.y) + 2.);

		FragColor=vec4(col, 1.0);
		FragColor = vec4(FragColor.xyz, .7);
	}
   
   if(sceltaFS == 5) {
		float t = time;
   		vec2 uv2 = (gl_FragCoord.xy - res.xy) / res.yy+0.7;
		uv2.x += 0.15;
	
		uv2.x *= abs(sin(uv2.x+t*2.0+22.0)*0.7+4.4);
		uv2.y *= abs(sin(uv2.x+t*2.0)+2.2)+1.0;
	
		vec3 col = vec3(uv2.y)*vec3(0.0,0.0, 0.0);
		float d = 1.0;
	
		float w = 0.01;
		//col = mix(vec3(1,uv2.y,0.5),col,smoothstep(lineWidth-w,lineWidth+w,d));

		vec2 uv = (gl_FragCoord.xy / res.xy);
		vec2 sunuv = (gl_FragCoord.xy * 2.0 - res) / min(res.x, res.y);

		float v = 0.0;
	
		vec3 sun = vec3(179, 107, 0) / 255.0;
	
		if (distance(sunuv, vec2(0.0)) > 0.5)
			sun = vec3(0.0);
		

		float n0 = noise(uv.x * 3.18, 0.3);
		float n1 = noise(uv.x * 2.34, 0.4);
		float n2 = noise(uv.x * 2.14, 0.6);
		float n3 = noise(uv.x * 1.4, 0.8);
		float n4 = noise(uv.x * 1.23, 1.0);
	
		if (uv.y < 0.2 + n4) {
			v = 0.5;
		} else if (uv.y < 0.3 + n3) {
			v = 0.4;
		} else if (uv.y < 0.35 + n2) {
			v = 0.3;
		} else if (uv.y < 0.385 + n1) {
			v = 0.2;
		} else if (uv.y < 0.41 + n0) {
			v = 0.1;
		}

		const vec3 sky_high = vec3(255, 221, 128) / 255.0;
		const vec3 sky_low = vec3(255, 174, 107) / 255.0;
		const vec3 water = vec3(97, 160, 171) / 255.0;
		vec3 color = mix(sky_low, sky_high, uv.y) + sun;
	
		vec3 red = vec3(1.0,0.2,0.0);
		vec2 P0 = sunuv + vec2(0.0, 0.3);
		float r = (1.0 - length(P0));
		red = red * vec3(r);

		if (v > 0.0) {
			vec3 water_color = water * v;
			color = mix(mix(sky_high, sky_low, 0.5), water_color, clamp(1.0-(uv.y*uv.y*uv.y)*10, 0.0, 1.0)) + red;
		}
			color += col;
	
		FragColor = vec4(color, 1.0);

		float cloudspeed1 = 0.000015;
		float cloudspeed2 = 0.000020;
		float cloudspeed3 = 0.000005;
		float cloudspeed4 = 0.000007;
		float cloudspeed5 = 0.00001;


		vec2 clouduv1 = vec2(map(1 - time * cloudspeed1 + floor(time * cloudspeed1),0,1,-0.1,1.1), 0.8);
		vec2 clouduv2 = vec2(map(1 - time * cloudspeed2 + floor(time * cloudspeed2),0,1,-0.1,1.1), 0.7);
		vec2 clouduv3 = vec2(map(1 - time * cloudspeed3 + floor(time * cloudspeed3),0,1,-0.1,1.1), 0.9);
		vec2 clouduv4 = vec2(map(1 - time * cloudspeed4 + floor(time * cloudspeed4),0,1,-0.1,1.1), 0.65);
		vec2 clouduv5 = vec2(map(1 - time * cloudspeed5 + floor(time * cloudspeed5),0,1,-0.1,1.1), 0.76);

		if(distance(clouduv1, uv) < 0.05 ||distance(clouduv2, uv) < 0.05||distance(clouduv3, uv) < 0.05 || distance(clouduv4, uv) < 0.05 ||distance(clouduv5, uv) < 0.05){
			FragColor = mix(FragColor,vec4(1,1,1,1),0.3);
		}


   }
   if(sceltaFS == 6) {
	    vec2 p = gl_FragCoord.xy / res.xy;

		vec2 uv = p*vec2(res.x/res.y,1.0);
	
		float f = 0.0;
		uv *= 8.0;
        mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
		f  = 0.5000*noise( uv ); uv = m*uv;
		f += 0.2500*noise( uv ); uv = m*uv;
		f += 0.1250*noise( uv ); uv = m*uv;
		f += 0.0625*noise( uv ); uv = m*uv;

		f = 0.5 + 0.5*f;
	
		f *= smoothstep( 0.0, 0.005, abs(p.x) );	
		f /= 7.0;
	
		FragColor = vec4(ourColor.r + f, ourColor.g+f, ourColor.b+f, 1);
   }

   if(sceltaFS == 7) {
		FragColor = vec4(1,1,0,1);
   }

}

