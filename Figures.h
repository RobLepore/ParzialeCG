#pragma once

#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"

class Body {
public: 
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLenum renderMode;
	unsigned int sceltaFS = 0;
	int nTriangles;

	vector<vec3> vertices;
	vector<vec4> colors;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	vec3 pos = vec3(0, 0, 0), scl = vec3(1, 1, 1);
	float angle = 0.0f;
	char* name;

	vec4 corner_b, corner_t;
	vec4 corner_b_obj, corner_t_obj;

	Body(char* name);

	virtual void show(unsigned int MatModel, unsigned int lsceltafs) = 0;

	void translate(vec3 pos);
	void scale(vec3 scl);
	void rotate(float);
	void move(float dx, float dy);
	void setBB();
};

class Figure : public Body {

public:
	Figure(int nTriangles, char* name);

	virtual void build() = 0;

	void show(unsigned int MatModel, unsigned int lsceltafs);

};

class Circle : public Figure {

public:
	vec4 centerColor = vec4(1,1,1,1);
	vec4 outerColor = vec4(0.5, 0.5, 0.5, 1);
	Circle(int nTriangles, char* name, vec4 centercol, vec4 outercol);

	Circle(int nTriangles, char* name);

	void build();
};

class Rect : public Figure {

public:
	vec4 swCol = vec4(0,0,0,0);
	vec4 nwCol = vec4(0, 0, 0, 0);
	vec4 neCol = vec4(0, 0, 0, 0);
	vec4 seCol = vec4(0, 0, 0, 0);

	Rect(char* name, vector<vec4> colors);

	Rect(char* name);

	void build();
};


class Hermite : public Figure {

private:
	typedef struct {
		GLuint VAO;
		GLuint VBO_G;
		GLuint VBO_C;
		int nTriangles;
		// Vertici
		vector<vec3> vertici;
		vector<vec3> CP;
		vector<vec4> colors;
		// Numero vertici
		int nv;
		//Matrice di Modellazione: Traslazione*Rotazione*Scala
		mat4 Model;
		string name;
	} Figura;

	Figura Poligonale, Derivata;
	float* t;

	float PHI0(float t) { return 2.0 * t * t * t - 3.0 * t * t + 1; };
	float PHI1(float t) { return t * t * t - 2.0 * t * t + t; };
	float PSI0(float t) { return -2.0 * t * t * t + 3.0 * t * t; };
	float PSI1(float t) { return t * t * t - t * t; };

	float dx(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
	{
		if (i == 0)
			return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i + 1].x - Fig->CP[i].x) / (t[i + 1] - t[i]);
		if (i == Fig->CP.size() - 1)
			return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i].x - Fig->CP[i - 1].x) / (t[i] - t[i - 1]);

		if (i % 2 == 0)
			return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
		else
			return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
	}
	float dy(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
	{
		if (i == 0)
			return 0.5 * (1.0 - Tens) * (1.0 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
		if (i == Fig->CP.size() - 1)
			return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]);

		if (i % 2 == 0)
			return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
		else
			return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	}
	float DX(int i, float* t)
	{
		if (Derivata.CP.at(i).x == 0)
			return dx(i, t, 0.0, 0.0, 0.0, &Poligonale);

		if (Derivata.CP.at(i).x != 0)
			return Derivata.CP.at(i).x;
	}
	float DY(int i, float* t)
	{
		if (Derivata.CP.at(i).y == 0)
			return dy(i, t, 0.0, 0.0, 0.0, &Poligonale);

		if (Derivata.CP.at(i).y != 0)
			return Derivata.CP.at(i).y;
	}

	void InterpolazioneHermite(float* t)
	{
		float p_t = 0, p_b = 0, p_c = 0, x, y;
		float passotg = 1.0 / (float)(140);

		float tg = 0, tgmapp, ampiezza;
		int i = 0;
		int is = 0; //indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg
					//appartiene


		vertices.clear();
		colors.clear();

		for (tg = 0; tg <= 1; tg += passotg)
		{
			//Localizzare l'intervallo t[is], t[is +1] a cui tg appartiene (tg � una sottodivisione pi� fine dell'intervallo)
			if (tg > t[is + 1]) is++;

			//Mappare tg nell'intervallo [0,1]
			ampiezza = t[is + 1] - t[is];
			tgmapp = (tg - t[is]) / ampiezza;

			x = CP[is].x * PHI0(tgmapp) + DX(is, t) * PHI1(tgmapp) * ampiezza + CP[is + 1].x * PSI0(tgmapp) + DX(is + 1, t) * PSI1(tgmapp) * ampiezza;
			y = CP[is].y * PHI0(tgmapp) + DY(is, t) * PHI1(tgmapp) * ampiezza + CP[is + 1].y * PSI0(tgmapp) + DY(is + 1, t) * PSI1(tgmapp) * ampiezza;

			vertices.push_back(vec3(x, y, 0.0));
			colors.push_back(color);
		}
	}

public:
	vec4 color;
	vector<vec3> CP;

	Hermite(char* _name, vector<vec3> controlPoints, vec4 _color);

	void build();

};

class Complex : public Body {

public:
	vector<Figure*> parts;

	Complex(char* _name);

	Complex(char* _name, Figure* f);

	void add(Figure* f);

	void updateBB();
	
	void show(unsigned int MatModel, unsigned int lsceltafs);
};