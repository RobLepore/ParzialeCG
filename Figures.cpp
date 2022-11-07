#include"Figures.h"

// ///////////// class Body  ///////////// //
Body::Body(char* _name) {
	printf("Init Body %s\n", _name);
	Model = mat4(1.0);
	name = _name;
}

void Body::translate(vec3 _pos) {
	pos = _pos;
}

void Body::scale(vec3 _val) {
	scl = _val;
}

void Body::rotate(float t) {
	angle = t;
}

void Body::move(float dx, float dy) {
	pos.x += dx;
	pos.y += dy;
}

void Body::setBB() {
	//Costruisco xmin,ymin, xmax,ymax peer identificare il Bounding Box del Proiettile
	int i;
	float xmax = -10000;
	float xmin = 10000;
	float ymax = -10000;
	float ymin = 10000;
	for (i = 1; i < nv; i++){
		if (vertices[i].x < xmin)
			xmin = vertices[i].x;
	}

	for (i = 1; i < nv; i++){
		if (vertices[i].x > xmax)
			xmax = vertices[i].x;
	}

	for (i = 1; i < nv; i++){
		if (vertices[i].y <= ymin)
			ymin = vertices[i].y;
	}

	for (i = 1; i < nv; i++){
		if (vertices[i].y > ymax)
			ymax = vertices[i].y;
	}
	//Aggiorno i valori del corner pi� in basso a sinistra (corner_b) e del corner pi� in alto a destra (conrner_t)

	corner_b_obj = vec4(xmin, ymin, 0.0, 1.0);
	corner_t_obj = vec4(xmax, ymax, 0.0, 1.0);

}


// ///////////// class Figure  ///////////// //
Figure::Figure(int _nTriangles, char* _name) : Body(_name) {
	nTriangles = _nTriangles;
}

void Figure::show(unsigned int MatModel, unsigned int lsceltafs) {
	Model = mat4(1.0);
	Model = glm::translate(Model, pos);
	Model = glm::scale(Model, scl);
	Model = glm::rotate(Model, angle, vec3(0, 0, 1));

	glUniform1i(lsceltafs, sceltaFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model)); // dice che matModel corrisponde a Scena[k].Model
	glBindVertexArray(VAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(renderMode, 0, nv);

}



// ///////////// class Circle  ///////////// //
Circle::Circle(int _nTriangles, char* _name, vec4 _centercol, vec4 _outercol) : Figure(_nTriangles, _name) {
	centerColor = _centercol;
	outerColor = _outercol;
	renderMode = GL_TRIANGLE_FAN;
	build();
}

Circle::Circle(int _nTriangles, char* _name) : Figure(_nTriangles, _name) {
	renderMode = GL_TRIANGLE_FAN;
	build();
}

void Circle::build() {
	printf("Costruzione circle %s %d \n", name, nTriangles);

	float stepA = (2 * PI) / nTriangles;

	vertices.push_back(vec3(0.0, 0.0, 0.0));
	colors.push_back(centerColor);

	for (int i = 0; i <= nTriangles; i++)
	{
		float t = (float)i * stepA;
		vertices.push_back(vec3(cos(t), sin(t), 0.0));
		colors.push_back(outerColor);
	}
	nv = vertices.size();

	setBB();
}



// ///////////// class Rect ///////////// //
Rect::Rect(char* _name, vector<vec4> colors) : Figure(2, _name) {
	swCol = colors[0];
	nwCol = colors[1];
	neCol = colors[2];
	seCol = colors[3];
	renderMode = GL_TRIANGLES;
	build();
}

Rect::Rect(char* _name) : Figure(0, _name) {
	renderMode = GL_TRIANGLES;
	build();
}

void Rect::build() {
	printf("Costruzione rectangle %s %d \n", name, nTriangles);
	vec3 verticesarr[] = {
	vec3(0.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(1.0f,  1.0f, 0.0f),
	vec3(0.0f,0.0f,0.0f),
	vec3(1.0f,0.0f,0.0f),
	vec3(1.0f,1.0f,0.0f)
	};

	vec4 colori[] = {
		swCol,
		nwCol,
		neCol,
		swCol,
		seCol,
		neCol
	};

	for (int i = 0; i < 6; i++)
	{
		vertices.push_back(verticesarr[i]);
		colors.push_back(colori[i]);
	}
	nv = vertices.size();
	setBB();
}



// ///////////// class Hermite  ///////////// //
Hermite::Hermite(char* _name, vector<vec3> controlPoints, vec4 _color) : Figure(0, _name) {
	color = _color;
	CP = controlPoints;
	for (int i = 0; i < CP.size(); i++) {
		Derivata.CP.push_back(vec3(0.0, 0.0, 0.0));
	}

	renderMode = GL_TRIANGLE_FAN;
	build();
}

void Hermite::build() {
	Poligonale.CP = CP;

	if (Poligonale.CP.size() > 1)
	{
		t = new float[CP.size()];
		int i;
		float step = 1.0 / (float)(CP.size() - 1);

		for (i = 0; i < CP.size(); i++)
			t[i] = (float)i * step;
		InterpolazioneHermite(t);

		nv = vertices.size();
		setBB();
	}
}


// ///////////// class Complex  ///////////// //
Complex::Complex(char* _name) : Body(_name) {}
Complex::Complex(char* _name, Figure *f) : Body(_name) {
	add(f);
}

void Complex::add(Figure* f) {
	parts.push_back(f);
	updateBB();
}

void Complex::updateBB() {
	//printf("\nVertici di %s\n", name);
	vertices.clear();
	for (int i = 0; i < parts.size(); i++) {
		//printf("parte: %s\n pos: %f %f\n", parts.at(i)->name, parts.at(i)->pos.x, parts.at(i)->pos.y);

		parts.at(i)->Model = mat4(1.0);
		parts.at(i)->Model = glm::rotate(parts.at(i)->Model, parts.at(i)->angle, vec3(0, 0, 1));
		parts.at(i)->Model = glm::translate(parts.at(i)->Model, parts.at(i)->pos);
		parts.at(i)->Model = glm::scale(parts.at(i)->Model, parts.at(i)->scl);

		for (int j = 0; j < parts.at(i)->vertices.size(); j++) {
			vec4 ver = parts.at(i)->Model * vec4(parts.at(i)->vertices.at(j), 1);
			//printf("%f %f\n", ver.x, ver.y);
			vertices.push_back(vec3(ver.x, ver.y, 0));
		}
	}
	nv = vertices.size();
	setBB();
}

void Complex::show(unsigned int MatModel, unsigned int lsceltafs) {
	Model = mat4(1.0);
	Model = glm::translate(Model, pos);
	Model = glm::scale(Model, scl);

	for (int i = 0; i < parts.size(); i++) {
		parts.at(i)->Model = Model;
		parts.at(i)->Model = glm::translate(parts.at(i)->Model, parts.at(i)->pos);
		parts.at(i)->Model = glm::scale(parts.at(i)->Model, parts.at(i)->scl);
		parts.at(i)->Model = glm::rotate(parts.at(i)->Model, parts.at(i)->angle, vec3(0, 0, 1));

		glUniform1i(lsceltafs, parts.at(i)->sceltaFS);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(parts.at(i)->Model));
		glBindVertexArray(parts.at(i)->VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(parts.at(i)->renderMode, 0, parts.at(i)->nv);


	}
}