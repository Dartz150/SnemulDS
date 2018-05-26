/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

//DSWNIFI Library revision: 1.2
#include "specific_shared.h"
#include "wifi_shared.h"
#include "clockTGDS.h"
#include "ipcfifoTGDS.h"

#ifdef ARM9
#include "dswnifi_lib.h"

#include "dswnifi.h"
#include "wifi_arm9.h"
#include "dswifi9.h"
#include "wifi_shared.h"
#include "utilsTGDS.h"
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <socket.h>
#include <in.h>
#include <assert.h>
#include <string.h>

//emu specific
#include "cfg.h"
#include "gfx.h"
#include "core.h"
#include "snes.h"
#include "engine.h"
#include "apu.h"
#include "ppu.h"
#include "main.h"
#include "opcodes.h"
#include "conf.h"
#include "fs.h"
#include "memmap.h"
#include "crc32.h"
#include "guiTGDS.h"
#include "apu_jukebox.h"

#include "dswnifi.h"
#include "dswnifi_lib.h"

#endif

#ifdef ARM9

//These methods are template you must override (as defined below), to have an easy DS - DS framework running.

//Example Sender Code
//Send This DS Time to External DS through UDP NIFI or Local NIFI:
//volatile uint8 somebuf[128];
//sprintf((char*)somebuf,"DSTime:%d:%d:%d",getTime()->tm_hour,getTime()->tm_min,getTime()->tm_sec);
//if(!FrameSenderUser){
//				FrameSenderUser = HandleSendUserspace((uint8*)somebuf,sizeof(somebuf));	//make room for crc16 frame
//}

//Example Receiver Code
__attribute__((section(".itcm")))
void HandleRecvUserspace(struct frameBlock * frameBlockRecv){
	//frameBlockRecv->framebuffer	//Pointer to received Frame
	//frameBlockRecv->frameSize		//Size of received Frame
	do_multi(frameBlockRecv);		
}

//Multiplayer key binding/buffer shared code. For DS-DS multiplayer emu core stuff.
__attribute__((section(".itcm")))
bool do_multi(struct frameBlock * frameBlockRecv)
{
	//frameBlockRecv->framebuffer	//Pointer to received Frame
	//frameBlockRecv->frameSize		//Size of received Frame
	
	switch(getMULTIMode()){
		
		//single player, has no access to shared buffers.
		case(dswifi_idlemode):{
			plykeys1 = get_joypad() | 0x80000000;	//bit15 (second half word) is for ready bit, required by emu		
			//joypad1 ready state? start reading from 15th bit backwards.
			if (DMA_PORT[0x00]&1)
			{
				SNES.Joy1_cnt = 16;	//ok send acknowledge bit
				write_joypad1((plykeys1&0xffff)); //only use actual joypad bits
			}
		
			//write joy2
			if (CFG.mouse){
				read_mouse();
			}
			if (CFG.scope){
				read_scope();
			}
			return false;
		}
		break;
		
		//NIFI local
		case(dswifi_localnifimode):{
			
			////////////////////////////// physical nifi layer
			//receiver
			struct snemulDSNIFIUserMsg thissnemulDSNIFIUserMsgReceiver;
			//void * memcpy ( void * destination, const void * source, size_t num );
			memcpy((uint8*)&thissnemulDSNIFIUserMsgReceiver,(uint8*)frameBlockRecv->framebuffer, sizeof(struct snemulDSNIFIUserMsg));
			
			//handle cmds
			
			//both are guest: based on timestamp the oldest time is guest
			if(thissnemulDSNIFIUserMsgReceiver.cmdIssued == HOSTCMD_FROM_EXT_DS_UPDATESTEP1){
				struct tm * tmInst = getTime();
				//host time is highest than ours: we guest
				if(
					(thissnemulDSNIFIUserMsgReceiver.DSEXTTime.tm_hour >= tmInst->tm_hour)  //equal or more than
					&&
					(thissnemulDSNIFIUserMsgReceiver.DSEXTTime.tm_min >= tmInst->tm_min)		//equal or more than
					&&
					(thissnemulDSNIFIUserMsgReceiver.DSEXTTime.tm_sec >= tmInst->tm_sec)		//always more than
				){
					nifiHost = false;
				}
				else{
					nifiHost = true;
				}
				
				nifiSetup = true;	//host: nifi ok
				/*
				clrscr();
				if(nifiHost == true){
					printf("this DS is HOST");
				}
				else{
					printf("this DS is GUEST");
				}
				*/
				//issue response
				issueISHOSTACKCmd(nifiHost);
			}
			else if(thissnemulDSNIFIUserMsgReceiver.cmdIssued == HOSTCMD_FROM_EXT_DS_UPDATESTEP2){
				
				if(thissnemulDSNIFIUserMsgReceiver.host == true){
					nifiHost = false;
				}
				else{
					nifiHost = true;
				}
				
				nifiSetup = true;	//guest: nifi ok
				
				/*
				clrscr();
				if(nifiHost == true){
					printf("this DS is HOST");
				}
				else{
					printf("this DS is GUEST");
				}
				*/
			}
			else if(thissnemulDSNIFIUserMsgReceiver.cmdIssued == HOSTCMD_FROM_EXT_DS_RESETNIFI){
				nifiHost = false;
				nifiSetup = false;
			}
			
			
			///////////////////////////////////// emu layer
			if(nifiSetup == true){
				
			}
			
			return true;
		}
		break;
		
		//UDP NIFI
		case(dswifi_udpnifimode):{
			//todo
			return true;
		}
		break;
	
	}
	
	return false;
}

//logic:
//both are guest: based on timestamp the oldest time is guest

bool nifiHost = false;	//true = yes, false = no
bool nifiSetup = false; //true = yes, false = no

//sender
struct snemulDSNIFIUserMsg thissnemulDSNIFIUserMsgSender;

struct snemulDSNIFIUserMsg * forgeNIFIMsg(int keys, int DS_VCOUNT, bool host, int SNES_VCOUNT, uint32 cmdIssued,struct tm DSEXTTime){
	thissnemulDSNIFIUserMsgSender.keys = keys;
	thissnemulDSNIFIUserMsgSender.DS_VCOUNT = DS_VCOUNT;
	thissnemulDSNIFIUserMsgSender.host = host;
	thissnemulDSNIFIUserMsgSender.SNES_VCOUNT = SNES_VCOUNT;
	thissnemulDSNIFIUserMsgSender.cmdIssued = cmdIssued;
	thissnemulDSNIFIUserMsgSender.DSEXTTime = DSEXTTime;
	return (struct snemulDSNIFIUserMsg *)(&thissnemulDSNIFIUserMsgSender);
}


void issueISHOSTCmd(){
	if (getMULTIMode() == dswifi_localnifimode){
		struct snemulDSNIFIUserMsg * snemulDSNIFIUserMsgInst = forgeNIFIMsg(0, 0, 0, 0, HOSTCMD_FROM_EXT_DS_UPDATESTEP1,*getTime());
		volatile char somebuf[frameDSsize];	//use frameDSsize as the sender buffer size, any other size won't be sent.
		//void * memcpy ( void * destination, const void * source, size_t num );
		memcpy((uint8*)&somebuf[0], (const uint8*)snemulDSNIFIUserMsgInst, sizeof(struct snemulDSNIFIUserMsg));
		FrameSenderUser = HandleSendUserspace((uint8*)somebuf,sizeof(somebuf));
	}
}


void issueISHOSTACKCmd(bool hostorGuest){
	if (getMULTIMode() == dswifi_localnifimode){
		struct snemulDSNIFIUserMsg * snemulDSNIFIUserMsgInst = forgeNIFIMsg(0, 0, hostorGuest, 0, HOSTCMD_FROM_EXT_DS_UPDATESTEP2,*getTime());
		volatile char somebuf[frameDSsize];	//use frameDSsize as the sender buffer size, any other size won't be sent.
		//void * memcpy ( void * destination, const void * source, size_t num );
		memcpy((uint8*)&somebuf[0], (const uint8*)snemulDSNIFIUserMsgInst, sizeof(struct snemulDSNIFIUserMsg));
		FrameSenderUser = HandleSendUserspace((uint8*)somebuf,sizeof(somebuf));
	}
}


void resetNifi(){
	if (getMULTIMode() == dswifi_localnifimode){
		struct snemulDSNIFIUserMsg * snemulDSNIFIUserMsgInst = forgeNIFIMsg(0, 0, 0, 0, HOSTCMD_FROM_EXT_DS_RESETNIFI,*getTime());
		volatile char somebuf[frameDSsize];	//use frameDSsize as the sender buffer size, any other size won't be sent.
		//void * memcpy ( void * destination, const void * source, size_t num );
		memcpy((uint8*)&somebuf[0], (const uint8*)snemulDSNIFIUserMsgInst, sizeof(struct snemulDSNIFIUserMsg));
		FrameSenderUser = HandleSendUserspace((uint8*)somebuf,sizeof(somebuf));
		nifiHost = false;
		nifiSetup = false;
	}
}

//runs in loop
void donifi(){
	if (getMULTIMode() == dswifi_localnifimode){
		if(nifiSetup == false){
			//cmd: Both DS become host/guest
			issueISHOSTCmd();
		}
		else{
			//play/update code
		}
	}
}

#endif