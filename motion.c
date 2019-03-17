#include "motion_def.h"


int ganbatte_motion(HANDLE hComm,int mode){
	short time[4] = {0,40,40,30};
//	if (mode == JIKEN){
//		talk(CHEER, GANBATTE);
//	}
	int number = 2;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, ganbatte[cycle], ganbatte[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,ganbatte[number]);
	return 0;
}

int daijob_motion(HANDLE hComm,int mode){
	short time[4] = {0,50,30,30};
//	if (mode == JIKEN){
//		talk(CHEER, DAIJOB);
//	}
	int number = 2;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, daijob[cycle], daijob[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,daijob[number]);
	return 0;
}

int aa_motion(HANDLE hComm,int mode){
	short time[5] = {0,50,30,30,50};
//	if (mode == JIKEN){
//		talk(NEGATIVE, AA);
//	}
	int number = 3;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, aa[cycle], aa[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,aa[number]);
	return 0;
}

int taihen_motion(HANDLE hComm,int mode){
	short time[4] = {0,30,30,50};
//	if (mode == JIKEN){
//		talk(NEGATIVE, TAIHEN);
//	}
	int number = 3;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, taihen[cycle], taihen[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,taihen[number]);
	return 0;
}

int ee_motion(HANDLE hComm,int mode){
	short time[3] = {70,70,70};
//	if (mode == JIKEN){
//		talk(NEGATIVE, EE);
//	}
	int number = 2;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, ee[cycle], ee[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,ee[number]);
	return 0;
}

int che_motion(HANDLE hComm,int mode){
	short time[3] = {100,20,70};
//	if (mode == JIKEN){
//		talk(NEGATIVE, CHE);
//	}
	int number = 2;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, che[cycle], che[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1]* 10);
	}
	back(hComm,che[number]);
	return 0;
}

int yabai_motion(HANDLE hComm,int mode){
	short time = 30;

//	if (mode == JIKEN){
//		talk(NEGATIVE, YABAI);
//	}
	int number = 3;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, yabai[cycle], yabai[cycle+1], time);
		Sleep(time * 10);
	}
	back(hComm,yabai[number]);
	return 0;
}

int yoshi_motion(HANDLE hComm,int mode){
	short time = 100;
//	if (mode == JIKEN){
//		talk(POSITIVE, YOSHI);
//	}
	int number = 2;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, yoshi[cycle], yoshi[cycle+1], time);
		Sleep(time * 10);
	}
	back(hComm,yoshi[number]);
	return 0;
}

int yeah_motion(HANDLE hComm,int mode){
	short time = 50;
//	if (mode == JIKEN){
//		talk(POSITIVE, YEAH);
//	}
	int number = 1;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, yeah[cycle], yeah[cycle+1], time);
		Sleep(time * 10);
	}
	back(hComm,yeah[number]);
	return 0;
}

int sgoisgoi_motion(HANDLE hComm,int mode){
	short time = 25;
//	if (mode == JIKEN){
//		talk(POSITIVE, SGOISGOI);
//	}
	int number = 5;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, sgoisgoi[cycle], sgoisgoi[cycle+1], time);
		Sleep(time * 10);
	}
	back(hComm,sgoisgoi[number]);
	return 0;
}

int izoizo_motion(HANDLE hComm,int mode){
	short time[4] = { 70, 70, 90,70 };
//	if (mode == JIKEN){
//		talk(POSITIVE, IZOIZO);
//	}
	int number = 3;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, izoizo[cycle], izoizo[cycle+1], time[cycle+1]);
		Sleep(time[cycle+1 ] * 10);
	}
	back(hComm,izoizo[number]);
	return 0;
}

int yatta_motion(HANDLE hComm,int mode){
	short time = 40;
//	if (mode == JIKEN){
//		talk(POSITIVE, YATTA);
//	}
	int number = 3;
	for (int cycle = 0; cycle < number; cycle++){
		RSSmooth(hComm, yatta[cycle], yatta[cycle+1], time);
	}
    back(hComm,yatta[number]);
    return 0;
}
