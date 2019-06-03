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
#include <Bridge.h>
#include "Programmer.h"

#include <Akka.cpp>
#include <Native.cpp>
#include <Echo.cpp>
#include <Bridge.cpp>
#include <Mqtt.cpp>
#include <Sender.cpp>
#include <System.cpp>
#include <ConfigActor.cpp>
#include <Keyboard.h>

Log logger(1024);
ActorMsgBus eb;
void overrideConfig(Config& config,int argc, char **argv);


#define MAX_PORT	20

void logFunction(char* str,uint32_t length) {
	static	ofstream ofs("stm32prog.log", std::ios_base::out );
	ofs << str << '\n';
	ofs.flush();
}



int main(int argc, char **argv) {
	Sys::init();
	INFO("version : " __DATE__ " " __TIME__ );
	INFO(" switching output to logfile. ");
	logger.setOutput(logFunction);

	config.loadFile("stm32prog.json");
	logger.setLogLevel('I');
	logger.setOutput(logFunction);
	overrideConfig(config,argc,argv);
	config.saveFile("stm32prog.json");
	static MessageDispatcher defaultDispatcher(5, 10240, tskIDLE_PRIORITY + 1);
	static ActorSystem actorSystem(Sys::hostname(), defaultDispatcher);

	std::string url; //= config.root()["mqtt"]["url"] | "tcp://limero.ddns.net:1883";
	config.setNameSpace("mqtt");
	config.get("url",url,"tcp://limero.ddns.net:1883");
	ActorRef& mqtt =
	    actorSystem.actorOf<Mqtt>("mqtt", url.c_str());
	actorSystem.actorOf<System>("system", mqtt);
	ActorRef& bridge = actorSystem.actorOf<Bridge>("bridge", mqtt);
	ActorRef& keyboard = actorSystem.actorOf<Keyboard>("keyboard");
	actorSystem.actorOf<Programmer>("programmer", keyboard,bridge);
	config.saveFile("stm32prog.json");

	sleep(10000000); // needed to avoid lost ACtorRef ?

}
void overrideConfig(Config& config,int argc, char **argv) {
	int  opt;

	while ((opt = getopt(argc, argv, "f:m:p:t:b:")) != -1) {
		switch (opt) {
			case 'm': {
					config.setNameSpace("mqtt");
					config.set("url",optarg);
					break;
				}
			case 'c': {
					config.loadFile(optarg);
					break;
				}
			case 'p': {
					uint32_t br=115200;
					sscanf(optarg,"%d",&br);
					config.setNameSpace("programmer");
					config.set("programmingBaudrate",br);
					break;
				}
			case 't': {
					uint32_t br=115200;
					sscanf(optarg,"%d",&br);
					config.setNameSpace("programmer");
					config.set("terminalBaudrate",br);
					break;
				}
			case 'b': {
					config.setNameSpace("programmer");
					config.set("binFile",optarg);
					break;
				}
			default: {/* '?' */
					fprintf(stderr, "Usage: %s [-c configFile] [-b binFile] [-m mqttUrl] [-p programmingBaudrate] [-t terminalBaudrate]\n",
					        argv[0]);
					fprintf(stderr," binFile : ./main.bin \n");
					fprintf(stderr," mqttUrl : tcp://iot;exclipse.org:1883\n");
					sleep(3);
					exit(EXIT_FAILURE);
				}
		}
	}
}
