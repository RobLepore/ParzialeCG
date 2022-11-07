#pragma once

#include "Figures.h"

class Entity {
public:
	Complex* geometry;
	bool fixed;
	bool enemy;
	bool alive = true;

	char* name;
	int life = 10;
	vec3 vel;
	vec3 acc;

	vector<vec3> prevpos;

	Entity(Complex* geometry, bool fixed);
	void show(unsigned int MatModel, unsigned int lsceltafs);
	void update();
	void applyForce(vec3 f);
	static bool colliding(Entity* e1, Entity* e2);
	bool hit() {
		life -= 5;
		if (life <= 0) {
			alive = false;
			return true;
		}
		else {
			return false;
		}
	}

	static void resolveCollision(Entity* e1, Entity* e2);

	

};

class Enemy : public Entity {
public:
	Enemy(Complex* _geometry) : Entity(_geometry, false) {
		enemy = true;
	}

	
};