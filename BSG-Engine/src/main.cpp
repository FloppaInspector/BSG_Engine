#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<vector>
#include<string>
#include<fstream>

#include<RenderWindow.hpp>
#include<Entity.hpp>
#include<Math.hpp>

#define ACC 0.0007

using namespace std;

int n=0;
vector<Entity> blocks;

struct object{
	Entity obj;
	float speed;
	float accel;
	bool onGround;
};

vector<object> objects;

float gravityFactor = -1;

int vt[1280][720];

Entity player = Entity(Vector2f(0,0),NULL);
float playerSpeed = 1;
float playerDir = 0;
float playerGrav = 0;
float playerAcc = 0;
float playerGrounded = false;

void checkGrounded(){
	for(int i=0;i<objects.size();i++){
		if(objects[i].onGround == false & objects[i].accel == 0){
			objects[i].accel = ACC;
			objects[i].speed = 0;
		}
		else if(objects[i].onGround == true & objects[i].accel != 0){
			objects[i].accel = 0;
			objects[i].speed = 0;
		}
	}
	if(playerGrounded == false && playerAcc == 0){
		playerAcc = ACC;
		playerGrav = 0;
	}
	else if(playerGrounded == true && playerAcc != 0){
		playerAcc = 0;
		playerGrav = 0;
	}
}

void clearVT(){
	for(int i=0;i<1280;i++){
		for(int j=0;j<720;j++){
			vt[i][j]=0;
		}
	}
}

int main(int argc, char* args[]){
	fstream newfile;
   	newfile.open("src/input.txt");

	/*input rules:
	TYPES:
	
	object--------------------------
	B (x) (y) (size) (tex): BLOCK
	O (x) (y) (size) (tex): OBJECT
	P (x) (y) (speed): PLAYER
	
	factors-------------------------
	G (g): GRAVITY
	*/

	RenderWindow window("GAME vA1.0",1280, 720);

	player = Entity(Vector2f(0.0f,0.0f),window.loadTexture("res/gfx/PlayerBlockTexture.png"));

	if(!newfile) cout<<"Can't open file";
	char tempChar;
	float tempX, tempY, tempS;
	string tempT;
	Entity tempEnt=Entity(Vector2f(0.0f,0.0f),window.loadTexture("res/gfx/groundtexture2.png"));
	
	clearVT();

	while(true){
		newfile>>tempChar;
		if(tempChar == 'B'){
			newfile>>tempX>>tempY>>tempS>>tempT;
			blocks.push_back(Entity(Vector2f(tempX,tempY),window.loadTexture(tempT.data())));
			for(int i=0;i<32;i++){
				vt[int(tempX+i)][int(tempY)] = 3;
			}
			
		}
		else if(tempChar == 'O'){
			newfile>>tempX>>tempY>>tempS>>tempT;
			objects.push_back({Entity(Vector2f(tempX,tempY),window.loadTexture(tempT.data())), 0, 0, false});
			for(int i=0;i<32;i++){
				vt[int(tempX+i)][int(tempY)] = 3;
			}
		}
		else if(tempChar == 'G'){
			newfile>>gravityFactor;
		}
		else if(tempChar == 'E'){
			break;
		}
		else if(tempChar == 'P'){
			newfile>>tempX>>tempY>>playerSpeed;
			player = Entity(Vector2f(tempX,tempY),window.loadTexture("res/gfx/PlayerBlockTexture.png"));
		}
	}

	if(SDL_Init(SDL_INIT_VIDEO)>0){
		std::cout<<"SDL_INIT HAS FAILED. SDL_ERROR:"<<SDL_GetError()<<std::endl;
	}
	if(!(IMG_Init(IMG_INIT_PNG))){
		std::cout<<"IMG_INIT HAS FAILED> IMG_EROR:"<<SDL_GetError()<<std::endl;
	}

	SDL_Texture* groundTexture = window.loadTexture("res/gfx/groundtexture2.png");

	vector<Entity> groundPlatforms;
	for(int i=0;i<4;i++){
		groundPlatforms.push_back(Entity(Vector2f(32*i,32*3),groundTexture));
	}

	//Entity groundPlatform(0,100,groundTexture);


	//start of game

	bool gameRunning = true;

	SDL_Event event;
	bool canPress = true;

	while(gameRunning){
		while(SDL_PollEvent(&event)){

			if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
				switch(event.key.keysym.sym){
				case SDLK_a:
					playerDir -= 1;
					break;
				case SDLK_d:
					playerDir += 1;
					break;
				}
			}
			if(event.type == SDL_KEYUP){
				switch(event.key.keysym.sym){
				case SDLK_a:
					playerDir += 1;
					break;
				case SDLK_d:
					playerDir -= 1;	
					break;
				}
			}

			//cout<<playerDir;

			if(event.type == SDL_QUIT){
				gameRunning = false;
			}
		}
		window.clear();
		/*for(int i=0;i<groundPlatforms.size();i++){
			window.render(groundPlatforms[i]);
		}*/
		for(int i=0;i<blocks.size();i++){
			window.render(blocks[i]);
		}
		for(int i=0;i<objects.size();i++){
			window.render(objects[i].obj);
		}
		window.render(player);
		window.display();
		
		for(int i=0;i<32;i++){
			vt[int(player.pos.x)+i][int(player.pos.y)] = 0;
		}

		//cout<< int(player.pos.x + (playerSpeed * playerDir * 0.1));

		player.pos.x = max(0.1, player.pos.x + playerSpeed * playerDir * 0.1);
		player.pos.y-=playerGrav * gravityFactor;
		playerGrav+=playerAcc;	

		for(int i=0;i<32;i++){
			vt[int(player.pos.x)+i][int(player.pos.y)] = 3;
		}

		for(int i=0;i<32;i++){
			if(vt[int(player.pos.x)+i][int(player.pos.y) + 32] == 3 && playerGrounded == false){
				playerGrounded = true;
				break;
			}
		}
		for(int i=0;i<32;i++){
			if(vt[int(player.pos.x)+i][int(player.pos.y) + 32] == 0 && playerGrounded == true){
				playerGrounded = false;
				break;
			}
		}

		for(int i=0;i<objects.size();i++){
			for(int j=0;j<32;j++){
				vt[int(objects[i].obj.pos.x)+j][int(objects[i].obj.pos.y)] = 0;
			}

			objects[i].obj.pos.y-=objects[i].speed * gravityFactor;
			objects[i].speed+=objects[i].accel;

			for(int j=0;j<32;j++){
				vt[int(objects[i].obj.pos.x) + j][int(objects[i].obj.pos.y)] = 3;
			}
			for(int j=0;j<32;j++){
				if(vt[int(objects[i].obj.pos.x)+j][int(objects[i].obj.pos.y) + 32] == 3 && objects[i].onGround == false){
					objects[i].onGround = true;
					break;
				}
			}
			for(int j=0;j<32;j++){
				if(vt[int(objects[i].obj.pos.x)+j][int(objects[i].obj.pos.y) + 32] == 0 && objects[i].onGround == true){
					objects[i].onGround = false;
					break;
				}
			}	
		}
		checkGrounded();
	}
	newfile.close();
	window.cleanUp();
	SDL_Quit();
	return 0;
}