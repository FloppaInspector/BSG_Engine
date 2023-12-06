#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<Math.hpp>

class Entity{
public:
	Entity(Vector2f p_pos, SDL_Texture* p_tex);
	Vector2f& getPos(){
		return pos;
	}
	SDL_Texture* getTex();
	SDL_Rect getCurrentFrame();
	Vector2f pos;
private:
	SDL_Texture* tex;
	SDL_Rect currentFrame;
};