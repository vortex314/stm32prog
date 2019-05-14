
#include "Programmer.h"
#include "Base64.h"

Programmer::Programmer( ActorRef& mqtt,ActorRef& bridge,ActorRef& publisher)
	: _mqtt(mqtt),_bridge(bridge),_publisher(publisher) {
}

Programmer::~Programmer() {
}

void Programmer::preStart() {
	RemoteActorRef services("global/services",_bridge);
	eb.subscribe(self(),MessageClassifier(Label("global/services"),MsgClass("stm32programmer")));
	_mqtt.tell(Msg(Mqtt::Subscribe)("topic","src/global/services/stm32programmer"),self());

	_timer1 = timers().startPeriodicTimer("timer1", Msg("timer1"), 10000);
	stateSelect = & receiveBuilder()
	.match(MsgClass("check"),[this](Msg&) {
	}).build();

}

Receive& Programmer::createReceive() {

	return receiveBuilder()

	.match(LABEL("getIdReply"), [this](Msg& msg) {

	})

	.match(LABEL("getVersionReply"), [this](Msg& msg) {
	})

	.match(LABEL("getReply"), [this](Msg& msg) {
	})

	.match(LABEL("readMemoryReply"), [this](Msg& msg) {

	})

	.match(LABEL("writeMemoryReply"), [this](Msg& msg) {
	})

	.match(LABEL("writeUnprotectReply"), [this](Msg& msg) {
	})

	.match(LABEL("readUnprotectReply"), [this](Msg& msg) {
	})

	.match(LABEL("eraseAllReply"), [this](Msg& msg) {
	})

	.match(LABEL("resetFlashReply"), [this](Msg& msg) {
	})

	.match(LABEL("resetSystemReply"), [this](Msg& msg) {
	})

	.match(MsgClass("timer1"), [this](Msg& msg) {
	})

	.match(MsgClass::Properties(), [this](Msg& msg) {
	})

	.build();
}
