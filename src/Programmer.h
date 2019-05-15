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
		MsgBatch* _batch;
	public:
		Programmer(ActorRef&,ActorRef&,ActorRef& );
		virtual ~Programmer() ;
		void preStart();
		Receive& createReceive();

		void batchProgram();
		uint32_t  programmingState(Msg& msg) ;
};





#endif // PROGRAMMER_H
