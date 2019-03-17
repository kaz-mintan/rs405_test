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

void comm_conf(int fd)//通信設定のための関数
{
  // 4_22 int status;
  //4_22  struct termio tio;
  struct termios tio; //4_22追加

  //4_22コメントアウト status = ioctl (fd, TCGETA, &tio);//TCGETAで現在のシリアルポートの設定を取得する
  //4_22コメントアウト　if (status < 0) perror("TCGETA");

  bzero(&tio, sizeof(tio));//4_22追加　termio構造体のtioの全てを0にリセット

  // tio.c_cflag = 0;//一度0にセットして全てのフラグをリセットする．
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
  //特殊制御文字は c_cc 配列で定義される．

  tio.c_iflag = 0;  //入力モードの設定。Rawモードでの入力
  //c_iflag フィールドは基本的なターミナル入力の制御の設定に使われる．
 
  tio.c_oflag = 0;  //出力モードの設定。Rawモードでの出力
  //c_oflag フィールドはシステムの出力処理の取り扱いの設定に使われる．

  tio.c_lflag = 0; //ローカルモードの設定．端末の特性を制御
  //c_lflag フィールドはターミナルの制御に使用される．

  tcflush(fd, TCIFLUSH);//通信バッファクリア
  
  tcsetattr(fd,TCSANOW,&tio);//4_22追加

  //  status = ioctl (fd, TCSETA, &tio);//TCSETSで現在のシリアルポートの設定をセットする
  //if (status < 0) perror("TCSETA");
}

/*----------------------------------------------------------------------------*/
/*
 *	概要：サーボのトルクをON/OFFする
 *
 *	関数：int RSTorqueOnOff( int fd, short sMode )
 *	引数：
 *		int			fd		通信ポートのハンドル
 *		short			sMode		1:トルクON
 *									0:トルクOFF
 *	戻り値：
 *		0以上			成功
 *		0未満			エラー
 *
 */
int RSTorqueOnOff( int fd, short sMode )
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
  sendbuf[0]  = (unsigned char)0xFA;				// ヘッダー1
  sendbuf[1]  = (unsigned char)0xAF;				// ヘッダー2
  sendbuf[2]  = (unsigned char)SERVO_ID;			// サーボID
  sendbuf[3]  = (unsigned char)0x00;				// フラグ
  sendbuf[4]  = (unsigned char)0x24;				// アドレス(0x24=36)
  sendbuf[5]  = (unsigned char)0x01;				// 長さ(4byte)
  sendbuf[6]  = (unsigned char)0x01;				// 個数
  sendbuf[7]  = (unsigned char)(sMode&0x00FF);	// ON/OFFフラグ
  // チェックサムの計算
  sum = sendbuf[2];
  for( i = 3; i < 8; i++ ){
    sum = (unsigned char)(sum ^ sendbuf[i]);
  }
  sendbuf[8] = sum;								// チェックサム

  // 通信バッファクリア
  // PurgeComm( hComm, PURGE_RXCLEAR );
  tcflush(fd, TCIFLUSH);
  // 送信
  ret = write( fd, &sendbuf, 9);

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
/*----------------------------------------------------------------------------*/
/*
 *	概要：サーボの現在角度を取得する
 *
 *	関数：short RSGetAngle( int fd )
 *	引数：
 *		int			fd		通信ポートのハンドル
 *
 *	戻り値：
 *		0以上			サーボの現在角度(0.1度=1)
 *		0未満			エラー
 *
 */
short RSGetAngle( int fd )
{
  unsigned char	sendbuf[32];
  unsigned char	readbuf[128];
  unsigned char	sum;
  int				i;
  int				ret;
  unsigned long	len, readlen;
  short			angle;


  // ハンドルチェック
  if( !fd ){
    return -1;
  }

  // バッファクリア
  memset( sendbuf, 0x00, sizeof( sendbuf ));

  // パケット作成
  sendbuf[0]  = (unsigned char)0xFA;				// ヘッダー1
  sendbuf[1]  = (unsigned char)0xAF;				// ヘッダー2
  sendbuf[2]  = (unsigned char)SERVO_ID;			// サーボID
  sendbuf[3]  = (unsigned char)0x09;				// フラグ(0x01 | 0x04<<1)
  sendbuf[4]  = (unsigned char)0x00;				// アドレス(0x00)
  sendbuf[5]  = (unsigned char)0x00;				// 長さ(0byte)
  sendbuf[6]  = (unsigned char)0x01;				// 個数
  // チェックサムの計算
  sum = sendbuf[2];
  for( i = 3; i < 7; i++ ){
    sum = (unsigned char)(sum ^ sendbuf[i]);
  }
  sendbuf[7] = sum;								// チェックサム

  // 通信バッファクリア
  // PurgeComm( hComm, PURGE_RXCLEAR );
  tcflush(fd, TCIFLUSH);
  // 送信
  ret = write( fd, &sendbuf, 8);
 
  // 受信のために少し待つ
  //  Sleep( 500 );//Sleepはwindows専用で単位はミリ秒 0.5秒待つ
  usleep(500000);//100万分の一秒単位　0.5秒待つ
  //  sleep();//秒
  // 読み込み
  memset( readbuf, 0x00, sizeof( readbuf ));
  readlen = 26;
  len = 0;

  ret = read( fd, readbuf, readlen );
 
  // 受信データの確認
  sum = readbuf[2];
  for( i = 3; i < 26; i++ ){
    sum = sum ^ readbuf[i];
  }
  if( sum ){
    // チェックサムエラー
    return -3;
  }

  angle = ((readbuf[8] << 8) & 0x0000FF00) | (readbuf[7] & 0x000000FF);

  return angle;
}




int main()
{
  int fd;
  int ret = 0;

  fd = open (MODEMDEVICE, O_RDWR);//対象のデバイスを読み書き可能なモードでオープンする．
  //    * O_RDONLY 読み込み専用でオープン
  // * O_WRONLY 書き込み専用でオープン
  //* O_RDWR 読み込みと書き込み用にオープン
  //* O_APPEND 書き込みのたびに末尾に追加する
  //* O_CREAT ファイルが存在しない場合、新たなファイルを作成する
  //* O_TRUNC サイズを 0 バイトに切り捨てる 
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
  
  //現在角度を読み出す
  printf( "Read Servo Anglen\n" );
  ret = RSGetAngle( fd );
  printf( "Servo Anglen : %8.1f\n", ((float)ret/10.0f) );
   
  //サーボを移動
  ret = RSMove( fd, 900, 500);
  if( ret < 0 ){
    printf( "ERROR:Move ON failed[%x]\n", ret );
    goto ErrorEnd;
  }

  // サーボの移動を待つ
  printf( "Waitting...\n" );
  sleep( 6 );

  //現在角度を読み出す
  printf( "Read Servo Anglen\n" );
  ret = RSGetAngle( fd );
  printf( "Servo Anglen : %8.1f\n", ((float)ret/10.0f) );
   
 ErrorEnd:
  // トルクをOffする
  // トルク ON=1/OFF=0
  printf( "SEND Torque Off\n" );
  ret = RSTorqueOnOff( fd, 0 );
  printf( "close.");
  close(fd);//最後にデバイスを閉じる．

  return ret;
}
