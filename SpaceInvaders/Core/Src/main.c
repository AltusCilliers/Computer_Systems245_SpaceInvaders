/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Sprites.h"
#include "images.h"
#include "audio.h"
#include <stdlib.h>
#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t refresh=0;
volatile uint8_t cleardone=0;



#define Screen_Start (unsigned char*)0x20020000

uint32_t *ptrsprite;

typedef struct Aliens
{

	int alienx;
	int alieny;
	int aoriginx;
	int aoriginy;
	int health;
	int amissilex;
	int amissiley;

} aliens;

aliens alien[24];

typedef struct Barriers
{

	int barrierx;
	int barriery;
	int boriginx;
	int boriginy;
	int health;


} barriers;

barriers barrierA[12];
barriers barrierB[12];
barriers barrierC[12];

typedef int Bool;

#define TRUE 1;
#define FALSE 0;

uint8_t sprites[1344];

uint8_t invaderX[24];
uint8_t invaderY[24];

int play=0;
int fire=0;
int amove = 3;
int lives=3;
int score=0;
int aliensize=24;

int cright=1;
int cleft=0;

int shot=0;
int bottom=0;
int ashot=0;

int Xpos = 160;
int Ypos = 195-23;
int Xposprev;
int Yposprev;

int invXpos=0;
int invYpos=0;
int Ylaserprev;

int amissx;
int amissy;

int ixprev=0;
int iyprev=0;
int nextrow=0;

int Xlaser;
int Ylaser;


Bool w = FALSE;
Bool a = FALSE;
Bool s = FALSE;
Bool d = FALSE;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	/* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
	 */

	if(GPIO_Pin == GPIO_PIN_3)
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)){
			play=1;
		}
	}

	if(GPIO_Pin == GPIO_PIN_0)
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
			fire=1;
		}
	}

	//d
	if(GPIO_Pin == GPIO_PIN_9)
	{
		if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)){

			d=TRUE;
		}

		if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)){

			d=FALSE;
		}

	}

	//a
	if(GPIO_Pin == GPIO_PIN_10)
	{
		if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)){

			a=TRUE;
		}

		if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)){

			a=FALSE;
		}
	}


}


/*uint8_t test3[] = {
0,0,0,0,1,1,1,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,0,
1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,0,0,1,1,0,0,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,1,1,0,0,1,1,0,0,0,
0,0,1,1,0,1,1,0,1,1,0,0,
1,1,0,0,0,0,0,0,0,0,1,1
};*/



void displayscore(uint16_t val, uint32_t* screenptr)
{
	uint8_t digit = 0;
	uint32_t* digitptr;
	uint32_t* scrcopyptr;
	for (int i = 0; i < 5; i++)
	{
		digit = val % 10;
		val /= 10;

		scrcopyptr = screenptr;
		digitptr = (uint32_t*)(digits + (digit << 3));
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				*scrcopyptr++ = *digitptr++;
			}
			digitptr += 18;
			scrcopyptr += 78;

		}
		screenptr -= 2;
	}
}



void drawSprite(unsigned char* ptr_sprite, int x, int y, int sprite_w, int sprite_h)
{
	uint32_t *ptrscreen;
	uint32_t *ptrsprite;

	//draw sprite
	ptrscreen = (uint32_t*)(0x20020000 + x+320*y);
	ptrsprite = (uint32_t*)ptr_sprite;
	for(int i = 0; i < sprite_h; i++)
	{
		for(int j = 0; j < sprite_w/4; j++)
		{
			*ptrscreen++ = *ptrsprite++;
		}
		ptrscreen += (320/4 - sprite_w/4);
	}
}

void clearSprite(unsigned char* ptr_sprite, int xprev, int yprev, int sprite_w, int sprite_h)
{
	uint32_t *ptrscreen;


	//clear sprite
	ptrscreen = (uint32_t*)(0x20020000 + xprev+320*yprev);

	for(int i = 0; i < sprite_h; i++)
	{
		for(int j = 0; j < sprite_w/4; j++)
		{
			*ptrscreen++ = 0;
		}
		ptrscreen += (320/4 - sprite_w/4);
	}
}



void  clearScreen(){
	cleardone=0;
	uint32_t* ptrscreen = (uint32_t*)0x20020000;
	*((uint32_t*)ptrscreen) = 0;

	HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0,0x20020000,0x20020004,15999);

	while(!cleardone);

}

void  updateScreen(){

	drawSprite(scoretext2, 5,1, 52, 13);
	displayscore(score, (uint32_t*)(0x2002069C));
	drawSprite(lives1, 200,1, 48, 13);
	displayscore(lives, (uint32_t*)(0x2002075B));


	drawSprite(shooter2, Xpos,Ypos, 24, 23);



}

void alienFire()
{
	int select;
	if (ashot==0)
	{

		ashot=1;

		do
		{
			select = 0 + (rand() % (24-0+1));
		}while(alien[select].health!=1);





		amissx=alien[select].alienx+2;
		amissy=alien[select].alieny;


	}





	if(ashot==1){

		amissy = amissy + 5;
		drawSprite(laser,amissx,amissy, 4, 8);
	}

	if(amissy>185)
	{
		ashot=0;
	}
}




void drawPixel(uint8_t* screenPtr, uint16_t x, uint16_t y, uint8_t colorIndex)
{
	uint8_t colorCodes[] = {48, 44, 41, 32};
	*(screenPtr+ y*320 + x) = colorCodes[colorIndex];
}



/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	LDRSprites(sprites);

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2S3_Init();
	/* USER CODE BEGIN 2 */

	srand(time(NULL));

	//create enemies

	for(int j = 0; j<24 ; j++){

		if(j<=5)
		{

			alien[j].alienx = 15 + j*20 ;
			alien[j].alieny = 20;
			alien[j].health=1;

		}
		if((6<=j)&&(j<=11))
		{
			alien[j].alienx = 15 + (j-6)*20 ;
			alien[j].alieny = 35;
			alien[j].health=1;

		}
		if((12<=j)&&(j<=17))
		{
			alien[j].alienx = 15 + (j-12)*20 ;
			alien[j].alieny = 50;
			alien[j].health=1;

		}
		if((18<=j)&&(j<=23))
		{
			alien[j].alienx = 15 + (j-18)*20 ;
			alien[j].alieny = 65;
			alien[j].health=1;

		}

	}

	//create bunkers

	//barrierA
	for (int j = 0; j < 12; j++)
	{


		if(j<=5)
		{
			barrierA[j].barrierx =55+j*4;
			barrierA[j].barriery =150;
			barrierA[j].health=2;
		}
		if ((6<=j)&&(j<=11))
		{
			barrierA[j].barrierx =55+(j-6)*4;
			barrierA[j].barriery =154;
			barrierA[j].health=2;
		}


	}

	//barrierB
	for (int j = 0; j < 12; j++)
	{


		if(j<=5)
		{
			barrierB[j].barrierx =160+j*4;
			barrierB[j].barriery =150;
			barrierB[j].health=2;
		}
		if ((6<=j)&&(j<=11))
		{
			barrierB[j].barrierx =160+(j-6)*4;
			barrierB[j].barriery =154;
			barrierB[j].health=2;
		}




	}


	//barrierC
	for (int j = 0; j < 12; j++)
	{


		if(j<=5)
		{
			barrierC[j].barrierx =265+j*4;
			barrierC[j].barriery =150;
			barrierC[j].health=2;
		}
		if ((6<=j)&&(j<=11))
		{
			barrierC[j].barrierx =265+(j-6)*4;
			barrierC[j].barriery =154;
			barrierC[j].health=2;
		}



	}




	while(!play)
	{
		drawSprite(background,0,0, 320, 200);
	}
	clearSprite(background,0,0, 320, 200);


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{

		if((!bottom)&&(alien!=0)&&(lives>0))
		{
			if(refresh)
			{
				clearScreen();
				updateScreen();

				refresh=0;



				//move ship
				if(a && (Xpos>0)){
					Xpos-=5;
				}

				if(d && (Xpos+24<315)){
					Xpos+=5;
				}

				//fire missile
				if(fire==1)
				{

					if(shot!=1){
						HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)shootaudio, AUDIOLEN1);
						Xlaser=Xpos+10;
						Ylaser=Ypos;
						Ylaserprev=Ylaser;
						shot=1;
					}

					if(shot==1){

						Ylaser=Ylaser-5;
						drawSprite(laser, Xlaser,Ylaser, 4, 8);



						if(Ylaser<5){
							fire=0;
							shot=0;

						}
					}
				}


				alienFire();

				//collionTest();








				//draw barriers

				for (int j = 0; j < 12; j++)
				{

					if(barrierA[j].health!=0)
					{
						//test to see if player missile hits barrierA
						if((Xlaser-2<=barrierA[j].barrierx) && (Xlaser+2>=barrierA[j].barrierx) && (Ylaser-1<=barrierA[j].barriery) && (Ylaser+3>=barrierA[j].barriery))
						{

							barrierA[j].health-=1;

							fire=0;
							shot=0;
							Xlaser=0;
							Ylaser=0;

						}

						if(barrierA[j].health==2)
						{
							drawSprite(bunkerA,barrierA[j].barrierx,barrierA[j].barriery, 4, 4);

						}

						if(barrierA[j].health==1)
						{
							drawSprite(bunkerA2,barrierA[j].barrierx,barrierA[j].barriery, 4, 4);

						}

						//test if enemy missile hits barriers
						if((amissx-2<=barrierA[j].barrierx) && (barrierA[j].barrierx<=amissx+2) && (amissy-3<=barrierA[j].barriery) && (amissy+1>=barrierA[j].barriery))
						{
							barrierA[j].health-=1;
							ashot=0;
							amissx=0;
							amissy=0;
						}


					}

					//test to see if player missile hits barrierB
					if(barrierB[j].health!=0)
					{
						if((Xlaser-2<=barrierB[j].barrierx) && (Xlaser+2>=barrierB[j].barrierx) && (Ylaser-1<=barrierB[j].barriery) && (Ylaser+3>=barrierB[j].barriery))
						{

							barrierB[j].health-=1;

							fire=0;
							shot=0;
							Xlaser=0;
							Ylaser=0;

						}
						if(barrierB[j].health==2)
						{

							drawSprite(bunkerA,barrierB[j].barrierx,barrierB[j].barriery, 4, 4);

						}

						if(barrierB[j].health==1)
						{

							drawSprite(bunkerA2,barrierB[j].barrierx,barrierB[j].barriery, 4, 4);

						}

						//test if enemy missile hits barrierB
						if((amissx-2<=barrierB[j].barrierx) && (barrierB[j].barrierx<=amissx+2) && (amissy-3<=barrierB[j].barriery) && (amissy+1>=barrierB[j].barriery))
						{
							barrierB[j].health-=1;
							ashot=0;
							amissx=0;
							amissy=0;
						}
					}

					//test to see if player missile hits barrierC
					if(barrierC[j].health!=0)
					{
						if((Xlaser-2<=barrierC[j].barrierx) && (Xlaser+2>=barrierC[j].barrierx) && (Ylaser-1<=barrierC[j].barriery) && (Ylaser+3>=barrierC[j].barriery))
						{

							barrierC[j].health-=1;

							fire=0;
							shot=0;
							Xlaser=0;
							Ylaser=0;

						}
						if(barrierC[j].health==2)
						{

							drawSprite(bunkerA,barrierC[j].barrierx,barrierC[j].barriery, 4, 4);

						}

						if(barrierC[j].health==1)
						{

							drawSprite(bunkerA2,barrierC[j].barrierx,barrierC[j].barriery, 4, 4);

						}

						//test if enemy missile hits barrierB
						if((amissx-2<=barrierC[j].barrierx) && (barrierC[j].barrierx<=amissx+2) && (amissy-3<=barrierC[j].barriery) && (amissy+1>=barrierC[j].barriery))	  	 				  	 				  	 			{
							barrierC[j].health-=1;
							ashot=0;
							amissx=0;
							amissy=0;
						}
					}


				}



				//increment x and y aliens and collision test with sides
				for(int k = 0; k<24 ; k++)
				{
					if(alien[k].health!=0)
					{

						//test to see if player missile hits invader
						if((Xlaser-7<=alien[k].alienx) && (Xlaser+7>=alien[k].alienx) && (Ylaser-10<=alien[k].alieny) && (Ylaser+10>=alien[k].alieny))
						{

							alien[k].health=0;
							HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)hitaudio, AUDIOLEN2);
							fire=0;
							shot=0;
							score+=10;
							Xlaser=0;
							Ylaser=0;
							aliensize--;
						}

						//tests to see if enemy missile hits player

						if((amissx-20<=Xpos) && (Xpos<=amissx+5) && (amissy-2<=Ypos) && (amissy+2>=Ypos))
						{
							ashot=0;
							amissx=0;
							amissy=0;
							lives=lives-1;
							drawSprite(shooterdead, Xpos,Ypos, 32, 31);
							HAL_Delay(15);

							if(lives>1){
								HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)deadaudio, AUDIOLEN4);
							}
						}

						//test to see if invaders touch player
						if(alien[k].alieny+10>Ypos)
						{

							bottom=1;
							lives=0;

						}


						drawSprite(test3, alien[k].alienx,alien[k].alieny, 12, 8);



					}

				}

				int down = 0;
				for (int j = 0;j<24;j++)
				{

					if((alien[j].alienx+amove>305)||(alien[j].alienx+amove<10))
					{
						amove*=-1;
						down = 1;

						for(int i = 0; i<24 ; i++)
						{
							alien[i].alieny += 10;

						}
						break;
					}


				}
				if(down==0)
				{
					for(int i = 0; i<24 ; i++)
					{
						alien[i].alienx=alien[i].alienx + amove;

					}
				}







			}
		}

		if ((bottom==1)||(lives==0))
		{
			clearScreen();
			drawSprite(gameover,20,8, 280, 184);
			HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)gameoveraudio, AUDIOLEN3);
			bottom=0;
			lives=-1;
		}


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Macro to configure the PLL multiplication factor
	 */
	__HAL_RCC_PLL_PLLM_CONFIG(16);
	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
	PeriphClkInitStruct.PLLI2S.PLLI2SM = 16;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief I2S3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S3_Init(void)
{

	/* USER CODE BEGIN I2S3_Init 0 */

	/* USER CODE END I2S3_Init 0 */

	/* USER CODE BEGIN I2S3_Init 1 */

	/* USER CODE END I2S3_Init 1 */
	hi2s3.Instance = SPI3;
	hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
	hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_8K;
	hi2s3.Init.CPOL = I2S_CPOL_LOW;
	hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2S3_Init 2 */

	/* USER CODE END I2S3_Init 2 */

}

/** 
 * Enable DMA controller clock
 * Configure DMA for memory to memory transfers
 *   hdma_memtomem_dma2_stream0
 */
static void MX_DMA_Init(void) 
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* Configure DMA request hdma_memtomem_dma2_stream0 on DMA2_Stream0 */
	hdma_memtomem_dma2_stream0.Instance = DMA2_Stream0;
	hdma_memtomem_dma2_stream0.Init.Channel = DMA_CHANNEL_0;
	hdma_memtomem_dma2_stream0.Init.Direction = DMA_MEMORY_TO_MEMORY;
	hdma_memtomem_dma2_stream0.Init.PeriphInc = DMA_PINC_ENABLE;
	hdma_memtomem_dma2_stream0.Init.MemInc = DMA_MINC_ENABLE;
	hdma_memtomem_dma2_stream0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_memtomem_dma2_stream0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_memtomem_dma2_stream0.Init.Mode = DMA_NORMAL;
	hdma_memtomem_dma2_stream0.Init.Priority = DMA_PRIORITY_LOW;
	hdma_memtomem_dma2_stream0.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_memtomem_dma2_stream0.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_memtomem_dma2_stream0.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_memtomem_dma2_stream0.Init.PeriphBurst = DMA_PBURST_SINGLE;
	if (HAL_DMA_Init(&hdma_memtomem_dma2_stream0) != HAL_OK)
	{
		Error_Handler( );
	}

	/* DMA interrupt init */
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pins : PA0 PA1 PA2 PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PD8 PD9 PD10 PD11 */
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : PB4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
