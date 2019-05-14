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
class Programmer : public Actor {
		ActorRef& _mqtt;
		ActorRef& _bridge;
		ActorRef& _publisher;
		Label _timer1;
		Receive* stateSelect;
		Receive* stateTerminal;
		Receive* stateDownload;
	public:
		Programmer(ActorRef&,ActorRef&,ActorRef& );
		virtual ~Programmer() ;
		void preStart();
		Receive& createReceive();
};


#endif // PROGRAMMER_H
