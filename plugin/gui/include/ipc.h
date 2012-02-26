/*
	Copyright (C) 2004-2005 Li Yudong
*/
/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _IPCSOCKET_H_
#define _IPCSOCKET_H_

#ifdef __cplusplus                     
extern "C" {
#endif

//format of IPC message:
/*
 DWORD(message value )  +  DWORD(size of added ) +  added data
|------4------|+|-------4-------|+|.........

size of added include the DWORD itself

*/


void 
TerminateIpcSocket();
//===============================================================================
//****************************Server*********************************************
//===============================================================================


//Initial listen socket and create thread for server
BOOL
InitIpcServer();


//Process ipc message for sersver
BOOL 
ProcessIpcMessageServer(
	IpcHand hand, 
	int iMsg,  
	void* pAttachData 
);

BOOL 
CreateApplicationServer(
	IpcHand hand, 
	void* pAttachData
);


//Destroy an application
//socket id
// pAttachData iAttachSize no used 
BOOL 
DestroyApplicationServer(
	IpcHand hand, 
	void* pAttachData 
);



//Show Main Window
//para:
// socket id
// pAttachData iAttachSize no used 
BOOL 
ShowMainWinServer(
	IpcHand hand, 
	void* pAttachData 
);


//send new generaged clip region to confirm the client waitting process
//the content sent by this message is same like send clip change message
void 
SendShowMainWinAnsToClient(
	PlGUIAppStat pStat
);

//Hide main window
//Param
//  socket id
//  pAttachData iAttachSize no used 
BOOL 
HideMainWinServer(
	IpcHand hand, 
	void* pAttachData 
);


//send message to client
//parameter:
//1. application description pointer
//2. message type
//3. message added data
//4. size of added message data
BOOL 
SendMsgByServer(
	PlGUIAppStat pStat,
	int iMsg,
	void* pAttachData,
	int iAttachSize
);


//send clip region change message 
void 
SendClipRgnChangeMsg(
	PlGUIAppStat pStat
);

//recalculate clip regioin of all application main window under current window 
void 
RecalClipRgnUnderThis(
	const PlGUIAppStat pStat
);

//===============================================================================
//****************************client*********************************************
//===============================================================================

//Initial ipc socket for client
BOOL 
InitIpcSocketClient();


//process ipc message for client
BOOL 
ProcessIpcMessageClient(
	int iMsg,  
	void* pAttachData,
	int iAttachSize
);


//send message by client
BOOL 
SendMsgByClient(
	int iMsg,
	void* pAttachData,
	int iAttachSize
);


BOOL 
ProcessMouseMsgClient(
	int iMsg,
	int x, 
	int y
);


BOOL 
ProcessMouseDownMsgClient(
	int iMsg, 
	int x, 
	int y
);


BOOL 
ProcessMouseMoveMsgClient(
	int iMsg, 
	int x, 
	int y
);


BOOL 
ProcessMouseUpMsgClient(
	int iMsg, 
	int x, int y
);


void 
SendKeyDown2Client(
	BYTE byteKeyValue
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _IPCSOCKET_H_ */
