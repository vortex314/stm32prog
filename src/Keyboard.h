#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Akka.h>
class Keyboard : public Actor {

	public:
		static MsgClass keyPressed;
		Keyboard();
		virtual ~Keyboard();
		void preStart();
		Receive& createReceive();
		void configStdin();
};

#endif // KEYBOARD_H
