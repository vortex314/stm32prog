#include <stdio.h>
#include <Log.h>
#include <Config.h>
#include <thread>

#include "Akka.h"
#include <Echo.h>
#include <Mqtt.h>
#include <Bridge.h>
#include <NeuralPid.h>
#include <Sender.h>
#include <System.h>
#include <Publisher.h>
#include "Programmer.h"

#include <Akka.cpp>
#include <Native.cpp>
#include <Echo.cpp>
#include <Bridge.cpp>
#include <Mqtt.cpp>
#include <Sender.cpp>
#include <System.cpp>
#include <Publisher.cpp>
#include <ConfigActor.cpp>


Log logger(1024);
ActorMsgBus eb;
void overrideConfig(Config& config,int argc, char **argv);


#define MAX_PORT	20



int main(int argc, char **argv) {
	INFO(" MAIN task started");

	Sys::init();
	INFO("version : " __DATE__ " " __TIME__ "\n");

//TODO	config.loadFile("stm32prog.json");
	logger.setLogLevel('I');
	overrideConfig(config,argc,argv);
	std::string url= config.root()["mqtt"]["url"] | "tcp://iot.eclipse.org:1883";
	config.save();

	INFO(" starting microAkka test ");
	static MessageDispatcher defaultDispatcher(2, 10240, tskIDLE_PRIORITY + 1);
	static ActorSystem actorSystem(Sys::hostname(), defaultDispatcher);

	ActorRef& mqtt =
	    actorSystem.actorOf<Mqtt>("mqtt", url.c_str());
	ActorRef& system = actorSystem.actorOf<System>("system", mqtt);
	ActorRef& bridge = actorSystem.actorOf<Bridge>("bridge", mqtt);
	ActorRef& publisher = actorSystem.actorOf<Publisher>("publisher", mqtt);
	actorSystem.actorOf<Programmer>("programmer", mqtt,bridge,publisher);

}


void overrideConfig(Config& config,int argc, char **argv) {
	int  opt;

	while ((opt = getopt(argc, argv, "f:m:")) != -1) {
		switch (opt) {
			case 'm':
				config.setNameSpace("mqtt");
				config.set("host",optarg);
				break;
			case 'f':
//TODO				config.loadFile(optarg);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-f configFile] [-m mqttHost]\n",
				        argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}
