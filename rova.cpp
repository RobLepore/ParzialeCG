
#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"
#include "Figures.h"
#include "Entities.h"

#define JUMPSPEED 30
#define JUMPHEIGHT 100.0f

static unsigned int programId, MatProj, MatModel;
unsigned int lsceltafs, loc_time, loc_res;
mat4 Projection;
vec2 res;

int width = 1200, height = 600;

void drawScene();

void crea_VAO_Vector(Figure* fig)
{

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertices.size() * sizeof(vec3), fig->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_S.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}


// colori ///////////////////////////////////////////////////
vec4 sandColor = vec4(194.0, 178.0, 128.0, 255.0) / 255.0f;
vec4 green = vec4(15, 150, 0, 255) / 255.0f;
vec4 brown = vec4(71, 43, 31, 255) / 255.0f;
vec4 pink = vec4(255, 209, 250, 255) / 255.0f;
vec4 lightGrey = vec4(vec3(211) / 255.0f, 1);
vec4 darkGrey = vec4(vec3(120) / 255.0f, 1);
/////////////////////////////////////////////////////////////


vector<Body*> scene;
vector<Entity*> entities;

Rect r((char*)"rect 1", { pink,pink,pink,pink });
Circle circle(5,(char*)"circ 1", green, green);

Complex c((char *) "complex 1");
Entity ent(&c, false);

Rect r2((char*)"rect 2", { brown,brown,brown,brown });

Complex c2((char*)"complex 2");
Entity ent2(&c2, true);



void INIT_VAO(void)
{
	crea_VAO_Vector(&r);
	printf("\n%f %f\n", r.corner_b_obj.x, r.corner_b_obj.y);
	printf("%f %f\n", r.corner_t_obj.x, r.corner_t_obj.y);

	crea_VAO_Vector(&circle);
	circle.translate(vec3(0, 1, 0));
	circle.scale(vec3(0.5, 0.5, 0));

	c.translate(vec3(200, 500, 0));
	c.scale(vec3(200, 200, 1));
	c.add(&r);
	c.add(&circle);
	//scene.push_back(&c);

	entities.push_back(&ent);

	crea_VAO_Vector(&r2);
	c2.add(&r2);
	c2.translate(vec3(20, 20,0));
	c2.scale(vec3(300, 100, 0));
	entities.push_back(&ent2);


	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	lsceltafs = glGetUniformLocation(programId, "sceltaFS");
	loc_time = glGetUniformLocation(programId, "time");
	loc_res = glGetUniformLocation(programId, "res");

}


void resize(GLsizei w, GLsizei h) {

	float AspectRatio_mondo = (float)(width) / (float)(height);
	if (AspectRatio_mondo > w / h) {
		glViewport(0, 0, w, w / AspectRatio_mondo);
	}
	else {
		glViewport(0, 0, h * AspectRatio_mondo, h);
	}
	glutPostRedisplay();
}


void redraw(int val) {
	glutTimerFunc(60, redraw, 0);
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Progetto OpenGL");
	glutDisplayFunc(drawScene);
	glutTimerFunc(60, redraw, 0);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}


void drawScene(void)
{

	glViewport(0, 0, width, height);
	int i;
	res.x = (float)width;
	res.y = (float)height;

	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1f(loc_time, glutGet(GLUT_ELAPSED_TIME) + 50000);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glUniform2f(loc_res, res.x, res.y);

	/*for (int k = 0; k < scene.size(); k++) {
		scene[k]->show(MatModel, lsceltafs);

		printf("\n%s\n%f %f\n", c.name, (c.Model * c.corner_b_obj).x, (c.Model * c.corner_b_obj).y);
		printf("%f %f\n", (c.Model * c.corner_t_obj).x, (c.Model * c.corner_t_obj).y);

	}*/
	
	for (int k = 0; k < entities.size(); k++) {
		entities[k]->acc = vec3(0, -0.5, 0);
		entities[k]->update();
		entities[k]->show(MatModel, lsceltafs);

		for (int j = 0; j < entities.size(); j++) {
			if (Entity::colliding(entities[k], entities[j]) && entities[k] != entities[j]) {
				printf("Collisione! %s %s\n", entities[k]->name, entities[j]->name);
			}
		}
	}
	glutSwapBuffers();
}