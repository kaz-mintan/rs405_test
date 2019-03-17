#include <termio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
//rawモードにするために追加したヘッダとdefine
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX 準拠のソース */
#define BYTE unsigned char

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>        

#define SERVO_ID		1			// サーボIDの指定「1」に設定
#define SERVO_ID		2			// サーボIDの指定「2」に設定
#define SERVO_ID		3			// サーボIDの指定「3」に設定

void comm_conf(int fd)//通信設定のための関数
{
  struct termios tio; //4_22追加

  bzero(&tio, sizeof(tio));//4_22追加　termio構造体のtioの全てを0にリセット

  tio.c_cflag |= (BAUDRATE | CS8 | CLOCAL | CREAD);
  //制御モードの指定。termios構造体member c_cflagでflagを指定
  //c_cflag フィールドターミナルのハードウェア制御の設定を行う．
  /*
    BAUDRATE: ボーレートの設定．cfsetispeed と cfsetospeed も使用できる．
    CRTSCTS : 出力のハードウェアフロー制御 (必要な結線が全てされているケー
    ブルを使う場合のみ．Serial-HOWTO の7章を参照のこと)
    CS8     :送受信8ビットを使う。 8n1 (8 ビット，ノンパリティ，ストップビット 1)
    CLOCAL  : modem stetus lineを無視する。ローカル接続，モデム制御なし
    CREAD   : 文字の受信を可能にする。受信文字(receiving characters)を有効にする．
  */

  tio.c_cc[VTIME] = 10;//バーストで短期のデータ伝送を
                       //タイムアウトするために使用する0.10秒単位のタイマ 単位は 100[msec]
  tio.c_cc[VMIN] = 0;//受信すべき最小の文字数
  tio.c_iflag = 0;  //入力モードの設定。Rawモードでの入力
  tio.c_oflag = 0;  //出力モードの設定。Rawモードでの出力
  tio.c_lflag = 0; //ローカルモードの設定．端末の特性を制御

  tcflush(fd, TCIFLUSH);//通信バッファクリア
  tcsetattr(fd,TCSANOW,&tio);//4_22追加

}

int RSTorqueOnOff(int fd, short sMode)//サーボ３つにするよ
{
	//unsigned char	sendbuf[28];
	unsigned char	sendbuf[28];
	unsigned char	sum;
	int				i;
	int				ret;
	unsigned long	len;

	// ハンドルチェック
	if (!fd){
		return -1;
	}

	// バッファクリア
	memset(sendbuf, 0x00, sizeof(sendbuf));

	//ロングパケットで書くよ
	sendbuf[0] = (unsigned char)0xFA;				// ヘッダー1
	sendbuf[1] = (unsigned char)0xAF;				// ヘッダー2
	//sendbuf[2] = (unsigned char)0x00;			// サーボID
	sendbuf[2] = (unsigned char)0x00;				// フラグ
	sendbuf[3] = (unsigned char)0x00;
	sendbuf[4] = (unsigned char)0x24;	//=36//
	sendbuf[5] = (unsigned char)0x02;	//Length 4byte
	sendbuf[6] = (unsigned char)0x03;	//number of servo motors
	sendbuf[7] = (unsigned char)SERVO_ID1; //first servo's id?
	sendbuf[8] = (unsigned char)(sMode & 0x00FF);
	sendbuf[9] = (unsigned char)SERVO_ID2;
	sendbuf[10] = (unsigned char)(sMode & 0x00FF);	// ON/OFFフラグ
	sendbuf[11] = (unsigned char)SERVO_ID3;
	sendbuf[12] = (unsigned char)(sMode & 0x00FF);
	sum = sendbuf[2];
	for (i = 3; i < 13; i++){//もしやばかったらここだから。//i<13に変更
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	//sendbuf[11] = sum;								// チェックサム
	sendbuf[13] = sum;

  // 通信バッファクリア
  // PurgeComm( hComm, PURGE_RXCLEAR );
  tcflush(fd, TCIFLUSH);
	// 送信
	ret = write(fd, &sendbuf, 14);//14に変更

	return ret;
}

/*----------------------------------------------------------------------------*/
/*
 *	概要：サーボを移動させる
 *
 *	関数：int RSMove( int fd, short sPos, unsigned short sTime )
 *	引数：
 *		int			fd		通信ポートのハンドル
 *		short			sPos		移動位置
 *		unsigned short	sTime		移動時間
 *	戻り値：
 *		0以上			成功
 *		0未満			エラー
 *
 */
int RSMove( int fd, short sPos, unsigned short sTime )
{
  unsigned char	sendbuf[28];
  unsigned char	sum;
  int				i;
  int				ret;
  
  // ハンドルチェック
  if( !fd ){
    return -1;
  }

  // バッファクリア
  memset( sendbuf, 0x00, sizeof( sendbuf ));

  // パケット作成
  sendbuf[0]  = (unsigned char)0xFA;				    // ヘッダー1
  sendbuf[1]  = (unsigned char)0xAF;				    // ヘッダー2
  sendbuf[2]  = (unsigned char)SERVO_ID;			    // サーボID
  sendbuf[3]  = (unsigned char)0x00;				    // フラグ
  sendbuf[4]  = (unsigned char)0x1E;				    // アドレス(0x1E=30)
  sendbuf[5]  = (unsigned char)0x04;				    // 長さ(4byte)
  sendbuf[6]  = (unsigned char)0x01;				    // 個数
  sendbuf[7]  = (unsigned char)(sPos&0x00FF);		    // 位置
  sendbuf[8]  = (unsigned char)((sPos&0xFF00)>>8);	// 位置
  sendbuf[9]  = (unsigned char)(sTime&0x00FF);	    // 時間
  sendbuf[10] = (unsigned char)((sTime&0xFF00)>>8);	// 時間
  // チェックサムの計算
  sum = sendbuf[2];
  for( i = 3; i < 11; i++ ){
    sum = (unsigned char)(sum ^ sendbuf[i]);
  }
  sendbuf[11] = sum;								// チェックサム

  // 通信バッファクリア
  //PurgeComm( hComm, PURGE_RXCLEAR );
  tcflush(fd, TCIFLUSH);
  // 送信
  ret = write( fd, &sendbuf, 12);

  return ret;
}

int RSMove(int fd, short sPos, short sPos2,short sPos3,unsigned short sTime)//sPos3を追加
{
	unsigned char	sendbuf[28];
	unsigned char	sum;
	int				i;
	int				ret;
	unsigned long	len;


	// ハンドルチェック
	if (!fd){
		return -1;
	}

	// バッファクリア
	memset(sendbuf, 0x00, sizeof(sendbuf));

	// パケット作成
	sendbuf[0] = (unsigned char)0xFA;				    // ヘッダー1
	sendbuf[1] = (unsigned char)0xAF;				    // ヘッダー2
	sendbuf[2] = (unsigned char)0x00;			    // サーボID
	sendbuf[3] = (unsigned char)0x00;   //flag
	sendbuf[4] = (unsigned char)0x1E;   //Adress(0x1E=30)
	sendbuf[5] = (unsigned char)0x5;   //length(4byte)
	sendbuf[6] = (unsigned char)0x03;   //number of motors
	sendbuf[7] = (unsigned char)SERVO_ID1; //first motor
	sendbuf[8] = (unsigned char)(sPos & 0x00FF);		    // VID1-1
	sendbuf[9] = (unsigned char)((sPos & 0xFF00) >> 8);	// VID1-2
	sendbuf[10] = (unsigned char)(sTime & 0x00FF);	    // VID1-3
	sendbuf[11] = (unsigned char)((sTime & 0xFF00) >> 8);	// VID1-4
	sendbuf[12] = (unsigned char)SERVO_ID2;
	sendbuf[13] = (unsigned char)(sPos2 & 0x00FF);		    // VID2-1
	sendbuf[14] = (unsigned char)((sPos2 & 0xFF00) >> 8);	// VID2-2
	sendbuf[15] = (unsigned char)(sTime & 0x00FF);	    // VID2-3
	sendbuf[16] = (unsigned char)((sTime & 0xFF00) >> 8);	// VID2-4
	sendbuf[17] = (unsigned char)SERVO_ID3;
	sendbuf[18] = (unsigned char)(sPos3 & 0x00FF);		    // VID2-1
	sendbuf[19] = (unsigned char)((sPos3 & 0xFF00) >> 8);	// VID2-2
	sendbuf[20] = (unsigned char)(sTime & 0x00FF);	    // VID2-3
	sendbuf[21] = (unsigned char)((sTime & 0xFF00) >> 8);	// VID2-4

	// チェックサムの計算
	sum = sendbuf[2];
	for (i = 3; i < 22; i++){
		sum = (unsigned char)(sum ^ sendbuf[i]);
	}
	sendbuf[22] = sum;								// チェックサム

  // 通信バッファクリア
  //PurgeComm( hComm, PURGE_RXCLEAR );
  tcflush(fd, TCIFLUSH);
  // 送信
  ret = write( fd, &sendbuf, 23);

	return ret;
}

void RSSmooth(int fd, short start[3], short goal[3], short time){
//void RSSmooth(HANDLE hComm, vector<double> start, vector<double> goal, double time){
	short dl[3];
	short xx[(int)TIME_SEP][3];
	int i;
	for (i = 0; i < 3; i++){
		dl[i] = (goal[i] - start[i])/(double)LEN_SEP;
		xx[0][i] = start[i];
	}
	short dt = time / TIME_SEP;
	const short array[(int)LEN_SEP] = {1,2,3,4,5,4,3,2,1};

	for (int cycle = 0; cycle < (int)TIME_SEP; cycle++){
		for (i = 0; i < 3; i++){
			xx[cycle+1][i] = xx[cycle][i]+dl[i]*array[cycle];
		}
		RSMove(fd, xx[cycle+1][0],xx[cycle+1][1],xx[cycle+1][2], dt);
		sleep(dt* 10);
	}
}

int back(int fd, short now[3]){
	short goal[3]={0,0,0};
	RSSmooth(fd, now, goal, 100);
	return 0;
}

int main()
{
  int fd;
  int ret = 0;

  fd = open (MODEMDEVICE, O_RDWR);//対象のデバイスを読み書き可能なモードでオープンする．
  printf("fd=%d\n", fd);//ファイルディスクリプタに3が割り当てられる。usbが繋がっていないと-1 (エラー)がfdに入る
  comm_conf(fd);//通信設定をする

  // トルクをONする
  // トルク ON=1/OFF=0
  printf( "SEND Torque ON\n" );
  ret = RSTorqueOnOff( fd, 1 );
  if( ret < 0 ){
    printf( "ERROR:Torque ON failed[%x]\n", ret );
    goto ErrorEnd;
  }
  
 ErrorEnd:
  // トルクをOffする
  // トルク ON=1/OFF=0
  printf( "SEND Torque Off\n" );
  ret = RSTorqueOnOff( fd, 0 );
  printf( "close.");
  close(fd);//最後にデバイスを閉じる．

  return ret;
}
