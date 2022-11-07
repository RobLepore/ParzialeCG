
#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"
#include "Figures.h"
#include "Entities.h"

#define GRAVITY 0.01
#define DT 22


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

int score = 0;
int spawnheight = height - 100;

// colori ///////////////////////////////////////////////////
vec4 sandColor = vec4(194.0, 178.0, 128.0, 255.0) / 255.0f;
vec4 green = vec4(15, 150, 0, 255) / 255.0f;
vec4 brown = vec4(71, 43, 31, 255) / 255.0f;
vec4 pink = vec4(255, 209, 250, 255) / 255.0f;
vec4 lightGrey = vec4(vec3(211) / 255.0f, 1);
vec4 darkGrey = vec4(vec3(120) / 255.0f, 1);
vec4 white = vec4(1, 1, 1, 1);
vec4 black = vec4(0, 0, 0, 1);
/////////////////////////////////////////////////////////////


vector<Body*> scene;
vector<Entity*> entities;

Rect background((char*)"mare", { vec4(0.8,0.8,0.8,1), vec4(0.8,0.8,0.8,1),vec4(0.8,0.8,0.8,1),vec4(0.8,0.8,0.8,1) });

Rect ground((char*)"ground geometry", { brown, green, green, brown });
Complex groundC((char*)"ground", &ground);
Entity groundEntity(&groundC, true);

Circle spell(30, (char*)"proiettile");
Complex spellC((char*)"spell", &spell);
Entity spellEntity(&spellC, true);

Complex mago((char*) "maghetto");
Entity magoEntity(&mago, false);

Hermite bastoneMago((char*)"bastone mago", {
	vec3(0,0,0),
	vec3(0.1,1,0),
	vec3(0.1,-1,0),
	vec3(-0.1,-1,0),
	vec3(-0.1,1,0),
	vec3(0.1,1,0),
	}, brown);

Complex enemy((char*)"cattivo");
Enemy enemyEntity(&enemy);

void creaBoundaries() {
	Rect* boundNorth = new Rect((char*)"limite sopra", { vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0) });
	Complex* boundNorthComplex = new Complex((char*)"limite sopra", boundNorth);
	Entity* boundNorthEntity = new Entity(boundNorthComplex, true);

	crea_VAO_Vector(boundNorth);
	boundNorthComplex->translate(vec3(0, height, 0));
	boundNorthComplex->scale(vec3(width, 50, 0));
	entities.push_back(boundNorthEntity);


	Rect* boundEast = new Rect((char*)"limite sopra", { vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0) });
	Complex* boundEastC = new Complex((char*)"limite destro", boundEast);
	Entity* boundEastE = new Entity(boundEastC, true);

	crea_VAO_Vector(boundEast);
	boundEastC->translate(vec3(-50, 0, 0));
	boundEastC->scale(vec3(50, height, 0));
	entities.push_back(boundEastE);



	Rect* boundWest = new Rect((char*)"limite sopra", { vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0), vec4(0,0,0,0) });
	Complex* boundWestC = new Complex((char*)"limite destro", boundWest);
	Entity* boundWestE = new Entity(boundWestC, true);

	crea_VAO_Vector(boundWest);
	boundWestC->translate(vec3(width, 0, 0));
	boundWestC->scale(vec3(50, height, 0));
	entities.push_back(boundWestE);
}

void creaMago() {
	Hermite* bodyMago = new Hermite((char*)"mago", {
	vec3(0,-1,0),
	vec3(-1,1,0),
	vec3(1,1,0),
	vec3(1,-1,0),
	vec3(-1.5,-1,0),
	vec3(-1.5,-0.5,0),
	vec3(-1,-0.5,0),
	vec3(-1,1,0)
		},
		vec4(0, 0, 1, 1));
	
	Hermite* hatMago = new Hermite((char*)"cappello mago", {
			vec3(0,0,0),
			vec3(1,0,0),
			vec3(0,-0.1,0),
			vec3(-1,0,0),
			vec3(-0.5,0.2,0),
			vec3(0,1.5,0),
			vec3(0.5,0.2,0),
			vec3(1,0,0),
		},
		vec4(0, 0, 1, 1));
	
	Hermite* beardMago = new Hermite((char*)"barba mago", {
		vec3(0,0,0),
		vec3(0,0.2,0),
		vec3(-1,0,0),
		vec3(0,-1,0),
		vec3(1,0,0),
		vec3(0,0.2,0),
		}, vec4(1, 1, 1, 1));
	
	Circle* headMago = new Circle(30, (char*)"testa mago", pink, pink);

	Rect* eye1 = new Rect((char*)"occhio mago 1", { black,black,black,black});
	Rect* eye2 = new Rect((char*)"occhio mago 2", { black,black,black,black });


	crea_VAO_Vector(bodyMago);
	bodyMago->translate(vec3(0, 0, 0));
	bodyMago->scale(vec3(1, 1, 1));

	crea_VAO_Vector(headMago);
	headMago->translate(vec3(0, 2, 0.0));
	headMago->scale(vec3(0.8, 1, 1));

	crea_VAO_Vector(hatMago);
	hatMago->translate(vec3(0, 2.5, 0));
	hatMago->scale(vec3(1.2, 1.3, 1));

	crea_VAO_Vector(beardMago);
	beardMago->translate(vec3(0, 1.5, 0));
	beardMago->scale(vec3(0.8, 1.5, 1));

	crea_VAO_Vector(eye1);
	eye1->translate(vec3(-0.6, 2.1, 0));
	eye1->scale(vec3(0.6, 0.3, 1));

	crea_VAO_Vector(eye2);
	eye2->translate(vec3(0.1, 2.1, 0));
	eye2->scale(vec3(0.6, 0.3, 1));




	mago.add(bodyMago);
	mago.add(headMago);
	mago.add(hatMago);
	mago.add(beardMago);
	//mago.add(eye1);
	//mago.add(eye2);


}
void creaEnemy() {
	Hermite* headEnemy = new Hermite((char*)"testa cattivo", {
	vec3(0,0,0),
	vec3(0,1,0),
	vec3(-1,2,0),
	vec3(-1,0,0),
	vec3(-1,-1,0),
	vec3(0,-2,0),
	vec3(1,-1,0),
	vec3(1,0,0),
	vec3(1,2,0),
	vec3(0,1,0),
		}, green);
	Hermite* bodyEnemy = new Hermite((char*)"corpo cattivo", {
		vec3(0, 0, 0),
		vec3(0.2, 1, 0),
		vec3(-0.2, 1, 0),
		vec3(-0.2, -1, 0),
		vec3(0.2, -1, 0),
		vec3(0.2, 1, 0),
		}, brown);
	Hermite* scarpa1 = new Hermite((char*)"scarpa 1", {
		vec3(0, 0, 0),
		vec3(-1, 1, 0),
		vec3(-1, 0, 0),
		vec3(-1.5, -0.5, 0),
		vec3(-1, -1, 0),
		vec3(1, -1, 0),
		vec3(1, 1, 0),
		vec3(-1, 1, 0),

		}, vec4(1, 0, 0, 1));
	Hermite* scarpa2 = new Hermite((char*)"scarpa 2", {
		vec3(0, 0, 0),
		vec3(-1, 1, 0),
		vec3(-1, 0, 0),
		vec3(-1.5, -0.5, 0),
		vec3(-1, -1, 0),
		vec3(1, -1, 0),
		vec3(1, 1, 0),
		vec3(-1, 1, 0),

		}, vec4(1, 0, 0, 1));

	vec4 black = vec4(0, 0, 0, 1);
	Hermite* eye1 = new Hermite((char*)"occhio 1", {
		vec3(0,0,0),
		vec3(-1,1,0),
		vec3(1,0,0),
		vec3(1, -1, 0),
		vec3(-1,0,0),
		vec3(-1,1,0)
		}, black);
	Hermite* eye2 = new Hermite((char*)"occhio 1", {
		vec3(0,0,0),
		vec3(-1,1,0),
		vec3(1,0,0),
		vec3(1, -1, 0),
		vec3(-1,0,0),
		vec3(-1,1,0)
		}, black);

	Circle* eye1s = new Circle(30, (char*)"eyes 1", black, black);
	Circle* eye2s = new Circle(30, (char*)"eyes 2", black, black);


	crea_VAO_Vector(headEnemy);
	headEnemy->translate(vec3(0, 2, 0));
	crea_VAO_Vector(bodyEnemy);
	bodyEnemy->scale(vec3(5, 1, 0));
	crea_VAO_Vector(scarpa1);
	scarpa1->translate(vec3(-0.5, -1, 0));
	scarpa1->scale(vec3(0.4, 0.5, 0));
	crea_VAO_Vector(scarpa2);
	scarpa2->translate(vec3(0.5, -1, 0));
	scarpa2->scale(vec3(0.4, 0.5, 0));
	crea_VAO_Vector(eye1);
	eye1->translate(vec3(-0.5, 2, 0));
	eye1->scale(vec3(0.3,0.3,1));
	crea_VAO_Vector(eye2);
	eye2->translate(vec3(0.5, 2, 0));
	eye2->scale(vec3(-0.3, 0.3, 1));
	Complex* newEnemyComplex = new Complex((char*)"nemico");
	crea_VAO_Vector(eye1s);
	eye1s->translate(vec3(-0.5, 1.8, 0));
	eye1s->scale(vec3(0.2, 0.2, 0));

	crea_VAO_Vector(eye2s);
	eye2s->translate(vec3(0.5, 1.8, 0));
	eye2s->scale(vec3(0.2, 0.2, 0));

	newEnemyComplex->translate(vec3(rand() % (width - 40) + 20, spawnheight, 0));
	newEnemyComplex->scale(vec3(15, 15, 0));

	newEnemyComplex->add(bodyEnemy);
	newEnemyComplex->add(scarpa1);
	newEnemyComplex->add(scarpa2);
	newEnemyComplex->add(headEnemy);
	newEnemyComplex->add(eye1);
	newEnemyComplex->add(eye2);
	newEnemyComplex->add(eye1s);
	newEnemyComplex->add(eye2s);




	Enemy* newEnemy = new Enemy(newEnemyComplex);
	entities.push_back(newEnemy);

	newEnemy->applyForce(vec3(0, GRAVITY, 0));
}
void creaPiattaforme() {
	Rect* plat = new Rect((char*)"plat", { brown, brown, brown, brown });
	Complex* platformGeom = new Complex((char*)"platform", plat);
	Entity* platform = new Entity(platformGeom, true);

	crea_VAO_Vector(plat);
	platformGeom->translate(vec3(100, 270, 0));
	platformGeom->scale(vec3(200, 20, 0));
	entities.push_back(platform);

	Rect* plat2 = new Rect((char*)"plat 2", { brown, brown, brown, brown });
	Complex* platformGeom2 = new Complex((char*)"platform 2", plat2);
	Entity* platform2 = new Entity(platformGeom2, true);

	crea_VAO_Vector(plat2);
	platformGeom2->translate(vec3(width - 100 - 200, 270, 0));
	platformGeom2->scale(vec3(200, 20, 0));
	entities.push_back(platform2);

}

void INIT_VAO(void)
{	

	creaBoundaries();

	crea_VAO_Vector(&background);
	background.translate(vec3(0, 0, 0.0));
	background.scale(vec3( width, height, 1));
	background.sceltaFS = 5;

	crea_VAO_Vector(&ground);
	groundC.translate(vec3(0, 0, 0.0));
	groundC.scale(vec3(width, 130, 1));
	groundC.add(&ground);
	ground.sceltaFS = 6;

	crea_VAO_Vector(&spell);
	spellC.translate(vec3(width / 2, 150, -10));
	spellC.scale(vec3(10, 10, 1));
	spellC.add(&spell);
	spell.sceltaFS = 7;
	spellEntity.alive = false;


	// /////  mago  ///// //
	creaMago();

	crea_VAO_Vector(&bastoneMago);
	bastoneMago.translate(vec3(0.5, 0.6, 0));
	bastoneMago.scale(vec3(1, 1.2, 1));
	mago.add(&bastoneMago);

	mago.translate(vec3(600, spawnheight, 0));
	mago.scale(vec3(20, 20, 1));


	////////////////////////

	creaPiattaforme();

	scene.push_back(&background);

	entities.push_back(&spellEntity);
	entities.push_back(&magoEntity);
	entities.push_back(&enemyEntity);
	entities.push_back(&groundEntity);

	

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
void ruotaBastone(int par) {

	if (bastoneMago.angle > -PI/4.0) {
		bastoneMago.angle-=0.2f;
		glutTimerFunc(DT, ruotaBastone, 0);
	}
	else {
		bastoneMago.angle = 0.0f;
	}
}
void jump() {
	magoEntity.geometry->move(0,5);
	magoEntity.applyForce(vec3(0,GRAVITY * 200,0));
}

bool shooting = false;
float spellSpeed = 5;
int spellSteps = 0;
void sparaSpell(int dir) {
	spellC.move(spellSpeed * dir, 0);
	spellSteps++;
	if(spellSteps < 50)
		glutTimerFunc(DT, sparaSpell, dir);
	else {
		spellSteps = 0;
		shooting = false;
		spellEntity.alive = false;
		spellC.translate(vec3(0,0,-10));
	}
}


void keyFunc(unsigned char key, int x, int y) {
	float step = 0.1;
	switch (key) {
	case 'a':
		magoEntity.geometry->move(0, 5);
		magoEntity.applyForce(vec3(-step, 0,0));
		mago.scl.x *= mago.scl.x > 0 ? -1 : 1;
		mago.updateBB();
		break;
	case 'd':
		magoEntity.geometry->move(0, 5);
		magoEntity.applyForce(vec3(step, 0, 0));
		mago.scl.x *= mago.scl.x > 0 ? 1 : -1;
		mago.updateBB();

		break;
	case 'e':
		if (!shooting) {
			shooting = true;
			spellEntity.alive = true;
			glutTimerFunc(DT, ruotaBastone, 0);
			spellC.translate(mago.pos + vec3(30 * (mago.scl.x > 0 ? 1 : -1), 0, 0));
			glutTimerFunc(DT, sparaSpell, mago.scl.x > 0 ? 1 : -1);
		}
		break;
	case ' ':
		jump();
		break;
	}
	//glutPostRedisplay();
}

void spawnEnemy(int par) {
	printf("qui\n");
	creaEnemy();
	glutTimerFunc(10 * 1000, spawnEnemy, 0);

}

void moveEnemies() {
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->enemy) {
			vec3 dir = mago.pos - entities[i]->geometry->pos;
			int dirx = (dir.x > 0 ? 1 : -1);
			entities[i]->geometry->move(0, 3);
			entities[i]->applyForce(vec3(0.05 * dirx, 0, 0));
		}
	}
	
}
void update(int par)
{
	glutTimerFunc(DT, update, 0);
	glutPostRedisplay();
}




int main(int argc, char* argv[])
{
	srand(time(NULL));


	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Progetto OpenGL");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyFunc);
	glutTimerFunc(DT, update, 0);
	glutTimerFunc(300, spawnEnemy, 0);


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
	moveEnemies();

	for (int s = 0; s < 10; s++) {

		for (int k = 0; k < scene.size(); k++) {
			scene[k]->show(MatModel, lsceltafs);
		}
		for (int k = 0; k < entities.size(); k++) {
			entities[k]->applyForce(vec3(0, -GRAVITY, 0));
			entities[k]->show(MatModel, lsceltafs);

			for (int j = 0; j < entities.size(); j++) {

				if (Entity::colliding(entities[k], entities[j]) && entities[k] != entities[j]) {
					//printf("Collisione! %s %s\n", entities[k]->name, entities[j]->name);
					if (!((entities[k]->name == "maghetto" && entities[j]->enemy) ||
						(entities[j]->name == "maghetto" && entities[k]->enemy) ||
						(entities[j]->enemy && entities[k]->enemy) ||
						(entities[j]->enemy && entities[k]->enemy))) {
						Entity::resolveCollision(entities[k], entities[j]);
					}
					else {
						//printf("lose life\n");
					}

					if (entities[k]->name == "spell" && entities[j]->enemy) {
						bool died = entities[j]->hit();
						if (died) {
							score += 10;
							printf("\nnew point! score: %d\n\n", score);
						}
						shooting = false;
						spellEntity.alive = false;
						spellC.translate(vec3(-100, -100, -100));
					}
				}
			}

			entities[k]->update();
		}
	}
	//printf("%.2f %.2f - %.2f %.2f\n", mago.corner_b.x, mago.corner_b.y, mago.corner_t.x, mago.corner_t.y);

	glutSwapBuffers();

}