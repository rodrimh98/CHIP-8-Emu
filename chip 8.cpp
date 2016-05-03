#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <stdlib.h>

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

	machine->sp = machine ->i = machine->dt = machine->st = 0x00;
	machine->pc = 0x200;
	for (int i = 0; i<MEMSIZE; i++)
		machine->mem[i]=0x00;

	for (int i = 0; i<16; i++){
		machine->stack[i] = 0x00;
		machine->v[i]= 0x00;
	}
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



int main (int argc, const char * argv[]){
	struct machine_t mac;
	init_machine(&mac);
	load_rom(&mac);
	//mac.pc = 0;
	cout<<"PC: "<<hex<<mac.pc<<endl;

	cout<<"&mac "<<&mac<<endl;
	uint16_t opcode;
	cout<<"Hex dump: \n";

	// main loop
	int quit = 0;
	while (!quit){

		opcode = mac.mem[mac.pc] << 8 | mac.mem[mac.pc + 1];
		cout<<hex<<opcode;
		mac.pc +=2;
		if ((mac.pc) == MEMSIZE){
			mac.pc = 0;
			//DEBUG ONLY!!!
			//quit=1;
		}

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
				cout<<"JP "<<nnn<<endl;
			break;

			case 2:
				cout<<"CALL "<<nnn<<endl;
			break;

			case 3: 
				cout<<"SE "<< x << kk <<endl;
			break;
		}
	}	
	return 0;

}