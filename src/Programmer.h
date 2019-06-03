#ifndef PROGRAMMER_H
#define PROGRAMMER_H

/*
 * Programmer.h
 *
 *  Created on: May 11, 2019
 *      Author: lieven
 */


#include <Akka.h>
#include <Bridge.h>
#include <Mqtt.h>
#include <vector>
#include <Base64.h>
#include "Keyboard.h"
#include <pt.h>
#include <Config.h>
class MsgBatch;
class Programmer : public Actor {
		ActorRef& _keyboard;
		ActorRef& _bridge;
		std::string _binFile;
		uint32_t _programmingBaudrate;
		uint32_t _terminalBaudrate;
		RemoteActorRef _stm32;
		RemoteActorRef _globalServices;
		Label _timer1;
		Receive* _stateSelect;
		Receive* _stateTerminal;
		Receive* _stateDownload;
		MsgBatch& _batch;
		enum {
			IDLE,PROGRAMMING,TERMINAL
		} _state;
		uint32_t _idCounter;
		bool _pingReplied;
		bool _prevPingReplied;
		uint32_t _idxBatchSend;
		uint32_t _idxBatchReply;
		Bytes _binary;
		struct pt _pt;
	public:
		Programmer(ActorRef&,ActorRef&);
		virtual ~Programmer() ;
		void preStart();
		Receive& createReceive();

		void batchProgram(Bytes& binImage);
		uint32_t  programming(struct pt* pt,Msg& msg) ;
		bool loadBinFile(Bytes& bytes,const char* binFile);
};





#endif // PROGRAMMER_H
