#include "Keyboard.h"
#include <linux/serial.h>
#include <termios.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>

MsgClass Keyboard::keyPressed("keyPressed");

Keyboard::Keyboard() {
}

Keyboard::~Keyboard() {
}

void Keyboard::configStdin() {
	struct termios options;

	if ( tcgetattr(0, &options)<0)
		ERROR("tcgetattr() failed  errno : %d : %s ", errno, strerror(errno));
	/*
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag &= ~HUPCL;  // avoid DTR drop at close time
		options.c_cflag |= CS8;
		options.c_cflag &= ~CRTSCTS; // Disable hardware flow control

	//	options.c_lflag &= ~(ECHO | ISIG); // no echo, signal
		options.c_lflag &= ICANON ; // wait full line
		options.c_cc[VEOL]='\n'; // add an additional EOL symbol
		options.c_iflag |= IGNCR; // ignore carriage return
	*/

	options.c_lflag &= ~(ECHO | ICANON);

	if ( tcsetattr(0, TCSAFLUSH, &options)<0)
		ERROR("tcsetattr() failed  errno : %d : %s ",errno, strerror(errno));
}

void Keyboard::preStart() {
	timers().startSingleTimer("timer",MsgClass("timer"),1000);
	configStdin();
}

Receive& Keyboard::createReceive() {
	return receiveBuilder()

	       .match(LABEL("timer"),
	[this](Msg& msg) {
		while(true) {
			uint8_t ch;
			int rc=read(0,&ch,1);
			if ( rc==1) {
				std::string str;
				str+=ch;
				eb.publish(Msg(keyPressed)("data",str));
			}
		}
	})

	.build();
}
