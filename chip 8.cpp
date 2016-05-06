#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

using namespace std;

#define MEMSIZE 4096

struct machine_t {
	uint8_t mem[MEMSIZE];
	uint16_t pc;

	uint8_t v[16];
	uint8_t st, dt;
	uint16_t i;

	uint16_t stack[16];
	uint8_t sp;
};

void init_machine (struct machine_t* machine){

	memset (machine, 0, sizeof(machine_t));
	machine->pc=0x200;
	}
static int load_rom (struct machine_t* machine){

	FILE* fp = fopen ("PONG", "rb");
	if (fp == NULL){
		cout<< "Error opening ROM \n";
		exit (1);
	}
	
	fseek (fp, 0, SEEK_END);
	int length = ftell(fp);
	fseek (fp,0, SEEK_SET);

	if (length > 3584 ){
		cout <<"Too large ROM \n";
		exit(1);
	}
	fread (machine->mem + 0x200, length, 1, fp);
	fclose(fp);

	cout <<"ROM has been loaded succesfully\n";
	cout <<"ROM size: "<<length<<" Bytes"<<endl;
	return 0;

}



void stepmachine (struct machine_t* cpu){
		uint16_t opcode;
		opcode = cpu->mem[cpu->pc] << 8 | cpu->mem[cpu->pc + 0x1];

		cout<<hex<<opcode;
		cpu->pc = (cpu->pc + 0x2)&0xfff;
		//cout<<cpu->pc<<endl;
		uint16_t nnn = opcode & 0x0FFF;
		uint8_t kk = opcode & 0xFF;
		uint8_t n = opcode & 0xF;
		uint8_t x = (opcode >> 8) & 0xF;
		uint8_t y = (opcode >> 4) & 0xF;
		uint8_t p = (opcode >> 12) & 0xF;

		//opcode assign 
		switch (p){
			case 0 :
				if (opcode == 0x00e0){
					cout<<"CLS\n";
				}
				else if (opcode == 0x00ee){
					cout<<"RET\n";				
				}
				

			break;

			case 1:
				cpu->pc = (cpu->pc + 2) & 0xfff;
				cout<<"JP "<<hex<<nnn<<endl;
			break;

			case 2:
				cout<<"CALL "<<nnn<<endl;
			break;

			case 3: 
				if (cpu->v[x]==kk)
					cpu->pc = (cpu->pc +2) & 0xfff;

				cout<<"SE "<<x<<" "<<kk<<endl;

			break;

			case 4:
				if (cpu->v[x] != kk)
					cpu->pc = (cpu->pc +2 ) & 0xfff;

				cout<<"SNE "<<x<<" "<<kk<<endl;

			break;

			case 5:
				if (cpu->v[x] == cpu->v[y])
					cpu->pc= (cpu->pc +2 ) & 0xfff;

				cout<<"SE "<<x<<" "<<y<<endl;
			break;

			case 6:
				cpu->v[x] = kk; 

				cout<<"LD "<<x<<" "<<kk<<endl;
			break;

			case 7:
				cpu->v[x] += kk;

				cout<<"ADD "<<x<<" "<<kk<<endl;
			break;

			case 8:
				switch (n){
					case 0:
						cpu->v[x] = cpu->v[y];
						cout<<"LD "<<x<<" "<<y<<endl;
					break;

					case 1:
						cpu->v[x] |= cpu->v[y];
						cout<<"OR "<<x<<" "<<y<<endl;
					break;

					case 2:
						cpu->v[x] &= cpu->v[y];
						cout<<"AND "<<x<<" "<<y<<endl;
					break;

					case 3:
						cpu->v[x] ^= cpu->v[y];
						cout<<"XOR "<<x<<" "<<y<<endl;
					break;

					case 4:
						cpu->v[0xf]=(cpu->v[x] > (cpu->v[x]+cpu->v[y]));
						cpu->v[x] += cpu->v[y];
						cout<<"ADD "<<x<<" "<<y<<endl;

					break;

					case 5:
						cpu->v[0xf]= (cpu->v[x] > cpu->v[y]);
						cpu->v[x] -= cpu->v[y];
						cout<<"SUB "<<x<<" "<<y<<endl;
					break;

					case 6:
						cpu->v[0xf] = cpu->v[x] & 0x01;
						cpu->v[x] = cpu->v[x] >> 1; 
						cout<<"SHR "<<x<<endl;
					break;

					case 7:
						cpu->v[0xf] = (cpu->v[y] > cpu->v[x]);
						cpu->v[x]= cpu->v[y]-cpu->v[x];
						cout<<"SUBN "<<x<<" "<<y<<endl;
					break;

					case 0xE:
						cpu->v[0xf] = ((cpu->v[x] & 0x80) != 0);
						cpu->v[x] = cpu->v[x] << 1;
						cout<<"SHL "<<x<<endl;
					break;

				}

			break;

			case 9:
				if (cpu->v[x] != cpu->v[y]);
					cpu->pc = (cpu->pc + 2) & 0xfff;

				cout<<"SNE "<<x<<" "<<y<<endl;
			break;

			case 0xA:
				cpu->i = nnn;
				cout<<"LD I "<<nnn<<endl;
			break;

			case 0xB:
				cpu->pc = cpu->v[0] + nnn;
				cout<<"JP "<<cpu->v[0]<<" "<<nnn<<endl;
			break;

			case 0xC:
				cout<<"RND "<< x << " " << kk<<endl;
			break;

			case 0xD:
				cout<<"DRW "<<x<<" "<<y<<" "<<n<<endl;
			break;
			
			case 0xE:
				switch(kk){
					case 0x9e:
						cout << "SKP "<< x<<endl;					
					break;

					case 0xA1:
						cout<<"SKNP "<<x<<endl;
					break;
				}
			break;

			case 0xF:
				switch (kk){
					case 0x07:
						cpu->v[x] = cpu->dt;
						cout<<"LD "<<x<<" dt"<<endl;
					break;

					case 0x0A:
						//fixme
						cout<<"LD "<<x<<endl;
					break;

					case 0x15:
						cpu->dt = cpu->v[x];
						cout<<"LD dt "<<x<<endl;
					break;

					case 0x18:
						cpu->dt= cpu->v[x];
						cout<<"LD st"<<x<<endl;
					break;

					case 0x1E:
						cpu->i += cpu->v[x];
						cout<<"ADD I "<<x<<endl;
					break;

					case 0x29:
						cout<<"LD F "<< x << endl;
					break;

					case 0x33:

						cout<<"LD B "<< x<<endl;
					break;

					case 0x55:
						cout<<"LD I "<<x<<endl;
					break;

					case 0x65:
						cout<<"LD "<<x<<" I"<<endl;
					break;
				}
			break;
		}
		}

int main (int argc, const char * argv[]){

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;
    SDL_Event event;
    struct machine_t mac;
    int mustQuit = 0;
 
    // Init emulator
    init_machine(&mac);
    load_rom(&mac);
    
    // Init SDL engine
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              640, 320, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING, 64, 32);
    
    // Write some dummy white background
    surface = SDL_CreateRGBSurfaceFrom(NULL, 64, 32, 32, 0,
                                       0x00FF0000,
                                       0x0000FF00,
                                       0x000000FF,
                                       0xFF000000);
    SDL_LockTexture(texture, NULL, &surface->pixels, &surface->pitch);
    memset(surface->pixels, 0xFF, surface->pitch * 32);
    SDL_UnlockTexture(texture);

    while (!mustQuit) {
        // step_machine(&mac);
        // Disabled. You'll see at the next live coding session why.

        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                mustQuit = 1;
                break;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    // Dispose SDL engine.
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;

}