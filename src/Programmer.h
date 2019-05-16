#ifndef PROGRAMMER_H
#define PROGRAMMER_H

/*
 * Programmer.h
 *
 *  Created on: May 11, 2019
 *      Author: lieven
 */


#include <Akka.h>
#include <Publisher.h>
#include <Mqtt.h>
#include <vector>
#include <Base64.h>
class MsgBatch;
class Programmer : public Actor {
		ActorRef& _mqtt;
		ActorRef& _bridge;
		ActorRef& _publisher;
		RemoteActorRef _stm32;
		Label _timer1;
		Receive* _stateSelect;
		Receive* _stateTerminal;
		Receive* _stateDownload;
		MsgBatch& _batch;
		enum {
			IDLE,PROGRAMMING,TERMINAL
		} _state;
	public:
		Programmer(ActorRef&,ActorRef&,ActorRef& );
		virtual ~Programmer() ;
		void preStart();
		Receive& createReceive();

		void batchProgram(Bytes& binImage);
		uint32_t  programmingState(Msg& msg,uint32_t window) ;
};





#endif // PROGRAMMER_H
