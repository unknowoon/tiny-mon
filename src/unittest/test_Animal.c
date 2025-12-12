/**
 * 구조체 = 클래스 (데이터 + 함수 포인터)
 * 함수 포인터 = 가상 함수 (다형성 구현)
 * 첫번째 인자로 self 전달 = this 포인터 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Queue(type, sound) \
    void type##_int(Animal *self) { \

}


typedef struct Animal {
	char name[32];
	int age;

	void (*speak)(Animal *self);
	void (*destroy)(Animal *self);
} Animal;

static void speak(Animal *self) {
	printf("[%s]: \'!@#$%^\'\n", self->name);
}

static void animal_destoy(Animal *self) {
	free(self);
}

static void animal_create(const char *name, int age) {
	Animal *animal = (Animal *)malloc(sizeof(Animal));
	if (!animal) return NULL;

	strncpy(animal->name, name, sizeof(animal->name)-1);
	animal->age = age;
	aninal->speak = speak;
	animal->destroy = destroy;

	return animal;
}



