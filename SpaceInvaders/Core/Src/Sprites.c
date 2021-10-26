/*
 * Author: R.J. Waller
 * Date: 24/08/2020
 * Version: 1.0
 */
#include "main.h"
#include "Sprites.h"


uint8_t Invader1[] = {
0,0,1,0,0,0,0,0,1,0,0,0,
0,0,0,1,0,0,0,1,0,0,0,0,
0,0,1,1,1,1,1,1,1,0,0,0,
0,1,1,0,1,1,1,0,1,1,0,0,
1,1,1,1,1,1,1,1,1,1,1,0,
1,0,1,1,1,1,1,1,1,0,1,0,
1,0,1,0,0,0,0,0,1,0,1,0,
0,0,0,1,1,0,1,1,0,0,0,0,
};

uint8_t Invader2[] = {
0,0,0,1,1,0,0,0,
0,0,1,1,1,1,0,0,
0,1,1,1,1,1,1,0,
1,1,0,1,1,0,1,1,
1,1,1,1,1,1,1,1,
0,0,1,0,0,1,0,0,
0,1,0,1,1,0,1,0,
1,0,1,0,0,1,0,1
};

uint8_t Invader3[] = {
0,0,0,0,1,1,1,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,0,
1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,0,0,1,1,0,0,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,1,1,0,0,1,1,0,0,0,
0,0,1,1,0,1,1,0,1,1,0,0,
1,1,0,0,0,0,0,0,0,0,1,1
};

uint8_t Ship[] = {
0,0,0,0,0,1,0,0,0,0,0,
0,0,0,0,1,1,1,0,0,0,0,
0,0,0,0,1,0,1,0,0,0,0,
0,0,1,0,1,0,1,0,1,0,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,1,1,1,1,1,1,1,0,
0,1,1,0,1,1,1,0,1,1,0,
0,0,0,0,0,1,0,0,0,0,0
};




uint8_t* Sprite2Mem(uint8_t *start, uint8_t *invader, uint8_t w, uint8_t h){
	uint8_t *invptr1 = invader;
	uint8_t *invptr2 = invader;
	uint8_t *memloc = start;

	//Invader1
	for(int r = 0; r < h; ++r){
		for(int c = 0; c < w; ++c){
			if(*invptr1 == 1){
				*memloc = 48;
				*(memloc+1) = 48;
			}else{
				*memloc = 0;
				*(memloc+1) = 0;
			}
			memloc += 2;
			invptr1++;
		}

		for(int c = 0; c < w; ++c){
			if(*invptr2 == 1){
				*memloc = 48;
				*(memloc+1) = 48;
			}else{
				*memloc = 0;
				*(memloc+1) = 0;
			}
			memloc += 2;
			invptr2++;
		}
	}

	return memloc;
}

void LDRSprites(uint8_t *start){
	uint8_t *memloc;
	memloc = Sprite2Mem(start, Invader1, 11, 8);
	memloc = Sprite2Mem(memloc, Invader2, 8, 8);
	memloc = Sprite2Mem(memloc, Invader3, 12, 8);
	memloc = Sprite2Mem(memloc, Ship, 11, 8);
}

uint8_t* FindSprite(uint8_t *start, uint8_t sprite){
	uint8_t *temp = start;

	switch(sprite){
	case 0:
		return temp;
	case 1:
		temp = temp + (INV1_W * INV1_H);
		return temp;
	case 2:
		temp = temp + (INV1_W * INV1_H) + (INV2_W * INV2_H);
		return temp;
	case 3:
		temp = temp + (INV1_W * INV1_H) + (INV2_W * INV2_H) + (INV3_W * INV3_H);
		return temp;
	}
	return temp;
}
