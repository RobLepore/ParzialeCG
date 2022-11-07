#include "Entities.h"
#include "Lib.h"

Entity::Entity(Complex* geom, bool _fixed) {
	geometry = geom;
	fixed = _fixed;
	name = geometry->name;
}

void Entity::show(unsigned int MatModel, unsigned int lsceltafs) {
	if (alive) {
		geometry->show(MatModel, lsceltafs);
	}
}

void Entity::applyForce(vec3 force) {
	acc += force;
}

void Entity::update() {
	if (fixed) return;
	vel += acc;
	if (enemy && vel.x > 0.2) {
		vel.x = 0.2;
	}
	geometry->pos += vel;
	//prevpos.push_back(geometry->pos);
	acc = vec3(0);
}

bool Entity::colliding(Entity* e1, Entity* e2) {
	if (!e1->alive || !e2->alive) return false;


	
	e1->geometry->corner_b = glm::scale(e1->geometry->Model, vec3(e1->geometry->scl.x > 0 ? 1 : -1,1,1)) * e1->geometry->corner_b_obj;
	e1->geometry->corner_t = glm::scale(e1->geometry->Model, vec3(e1->geometry->scl.x > 0 ? 1 : -1, 1, 1)) * e1->geometry->corner_t_obj;
	e2->geometry->corner_b = glm::scale(e2->geometry->Model, vec3(e2->geometry->scl.x > 0 ? 1 : -1, 1, 1)) * e2->geometry->corner_b_obj;
	e2->geometry->corner_t = glm::scale(e2->geometry->Model, vec3(e2->geometry->scl.x > 0 ? 1 : -1, 1, 1)) * e2->geometry->corner_t_obj;
	
	// guardo collisioni su asse x
	bool collisionX = e1->geometry->corner_b.x <= e2->geometry->corner_t.x &&
		e1->geometry->corner_t.x >= e2->geometry->corner_b.x;

	// guardo collisioni su asse y
	bool collisionY = e1->geometry->corner_b.y <= e2->geometry->corner_t.y &&
		e1->geometry->corner_t.y >= e2->geometry->corner_b.y;
	//Si ha collisione se c'è collisione sia nella direzione x che nella direzione y

	return collisionX && collisionY;
	
}

void Entity::resolveCollision(Entity* e1, Entity* e2) {
	e1->geometry->pos -= e1->vel;
	e1->vel = vec3(0);
	e1->acc = vec3(0);
	e2->geometry->pos -= e2->vel;
	e2->vel = vec3(0);
	e2->acc = vec3(0);
}