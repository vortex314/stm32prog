
#include "Programmer.h"
#include "Base64.h"
#include <pt.h>

class MsgBatch {
		std::vector<Msg> _batch;
		std::vector<Msg>::iterator _cursor;
	public:
		MsgBatch() {

		}
		void clear()  {
			_batch.clear();
		}

		uint32_t size() {
			return _batch.size();
		}
		Msg& at(uint32_t idx) {
			return _batch.at(idx);
		}
		void start() {
			_cursor=_batch.begin();
		}
};

Programmer::Programmer( ActorRef& mqtt,ActorRef& bridge,ActorRef& publisher)
	: _mqtt(mqtt),_bridge(bridge),_publisher(publisher),_stm32("ESP32-12857/programmer",bridge) {
	_batch=new MsgBatch();
}


Programmer::~Programmer() {
}

void Programmer::preStart() {
	RemoteActorRef services("global/services",_bridge);
	eb.subscribe(self(),MessageClassifier(Label("global/services"),MsgClass("stm32programmer")));
	_mqtt.tell(Msg(Mqtt::Subscribe)("topic","src/global/services/stm32programmer"),self());

	_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 10000);
	/*stateSelect = & receiveBuilder()
	.match(MsgClass("check"),[this](Msg&) {
	}).build();*/

}

uid_type replyCls(uid_type reqCls) {
	std::string replyStr = Label::label(reqCls);
	replyStr += "Reply";
	return Label(replyStr.c_str()).id();
}

void Programmer::batchProgram() {
	std::vector<Msg> msgs;
	msgs.push_back(Msg("ping"));
	msgs.push_back(Msg("resetSystem"));
	msgs.push_back(Msg("eraseAll"));
	//TODO add writeMemory
	msgs.push_back((const Msg)Msg("writeMemory")("addressHex","8000000")("data","ABCD"));
	msgs.push_back(Msg("readMemory")("addressHex","8000000"));
	msgs.push_back(Msg("resetFlash"));
}

uint32_t  Programmer::programmingState(Msg& msg) {
	struct pt pt;
	static uint32_t firstBatchMsg=0;
	PT_BEGIN(&pt);
//TODO send out window size msgs
//TODO wait reply in sequence, if timeout cancel batch
	_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 1000);
	PT_WAIT_UNTIL(&pt,msg.cls()==MsgClass("timeout").id() || msg.cls()==replyCls(_batch->at(firstBatchMsg).cls()));
	if ( msg.cls()==MsgClass("timeout").id() ) {
		PT_EXIT(&pt);
	} else {
		int erc;
		if ( msg.get("erc",erc)==0 && erc==E_OK )
			timers().cancel(_timer1);
		firstBatchMsg++;
	}
	PT_END(&pt);
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
