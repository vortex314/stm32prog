
#include "Programmer.h"
#include "Base64.h"
#include <pt.h>
#include <fstream>

class MsgBatch {
		std::vector<Msg> _batch;
		std::vector<Msg>::iterator _cursor;

	public:
		MsgBatch() {}
		void clear() { _batch.clear(); }
		uint32_t size() { return _batch.size(); }
		Msg& at(uint32_t idx) { return _batch.at(idx); }
		void start() { _cursor = _batch.begin(); }
		void add(Msg m) { _batch.push_back(m); }
};


Programmer::Programmer(ActorRef& keyboard, ActorRef& bridge, ActorRef& publisher)
	: _keyboard(keyboard)
	, _bridge(bridge)
	, _publisher(publisher)
	, _stm32("ESP32-12857/programmer", bridge)
	, _batch(*new MsgBatch()) {
	_state = IDLE;
	_idCounter=0;
}

Programmer::~Programmer() {}
void Programmer::preStart() {
	RemoteActorRef services("global/services", _bridge);
	eb.subscribe(self(), MessageClassifier(Label("global/services"), MsgClass("stm32programmer")));
//	_mqtt.tell(Msg(Mqtt::Subscribe)("topic", "src/global/services/stm32programmer"), self());

	_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 10000);
	timers().startPeriodicTimer("",Msg("pingTimer"),3000);
	_stateSelect = &receiveBuilder().match(MsgClass("check"), [this](Msg&) {}).build();

	eb.subscribe(self(),MessageClassifier(_keyboard,Keyboard::keyPressed));
}

uid_type replyCls(uid_type reqCls) {
	std::string replyStr = Label::label(reqCls);
	replyStr += "Reply";
	return Label(replyStr.c_str()).id();
}

bool Programmer::loadBinFile(Bytes& bytes,const char* binFile) {
	uint8_t buffer[256];
	FILE *ptr;

	bytes.clear();
	ptr = fopen(binFile,"rb");  // r for read, b for binary
	if ( ptr==0) return false;
	while(true) {
		int rc =fread(buffer,1,sizeof(buffer),ptr);
		bytes.write(buffer,0,rc);
		if ( rc != 256 ) break;
	}
	fclose(ptr);
	return true;
}

void Programmer::batchProgram(Bytes& binImage) {
	std::vector<Msg> msgs;
	_batch.add(Msg("ping").id(_idCounter++));
	_batch.add(Msg("resetSystem").id(_idCounter++));
	_batch.add(Msg("eraseAll").id(_idCounter++));
	// TODO add writeMemory
	uint32_t imageSize=binImage.length();
	uint32_t sectors = (imageSize / 256) ;
	sectors += ((imageSize % 256) == 0) ? 0 : 1;
	uint32_t address = 0x8000000;
	Bytes sectorData(256);
	std::string addressHex;
	std::string sectorBase64;
	for(uint32_t i = 0; i < sectors; i++) {
		string_format(addressHex, "%X", address + i * 256);
		binImage.offset(i * 256);
		while(binImage.hasData() && sectorData.hasSpace(1))
			sectorData.write(binImage.read());
		while(sectorData.length() % 4 != 0) sectorData.write(0xFF);
		Base64::encode(sectorBase64, sectorData);
		_batch.add(Msg("writeMemory")("addressHex", addressHex)("data", sectorBase64).id(_idCounter++));
	}
	_batch.add(Msg("readMemory")("addressHex", "8000000").id(_idCounter++));
	_batch.add(Msg("resetFlash").id(_idCounter++));
}

uint32_t Programmer::programming(Msg& msg) {
	static struct pt pt;
	static uint32_t firstBatchMsg = 0;
	uint32_t window = 5;
	PT_BEGIN(&pt);
	// TODO send out window size msgs
	// TODO wait reply in sequence, if timeout cancel batch
	for(uint32_t i = 0; i < window; i++) {
		_stm32.tell(_batch.at(i),self());
	}
	while(true) {
		_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 3000);
		PT_YIELD_UNTIL(&pt,
		               msg.cls() == MsgClass("timeout").id() ||
		               (msg.cls() == replyCls(_batch.at(firstBatchMsg).cls()) &&
		                !( msg.cls()==MsgClass("pingReply").id()) ));
		INFO("%s",msg.toString().c_str());
		if(msg.cls() == MsgClass("timeout").id()) {
			ERROR(" programming stopped , timeout encountered.");
			PT_EXIT(&pt);
		} else {
			int erc;
			if(msg.get("erc", erc) == 0 && erc == E_OK) {
				timers().cancel(_timer1);
				firstBatchMsg++;
				if(firstBatchMsg == _batch.size()) PT_EXIT(&pt);
				if(firstBatchMsg + window < _batch.size()) _stm32.tell(_batch.at(firstBatchMsg + window));
			} else {
				ERROR(" programming stopped , error encountered. %s ",msg.toString().c_str());
				PT_EXIT(&pt);
			}
		}
	}
	PT_END(&pt);
}

Receive& Programmer::createReceive() {
	return receiveBuilder()

	       .match(Keyboard::keyPressed,
	[this](Msg& msg) {
		std::string keys;
		if(msg.get("data", keys) == 0) {
			for(char ch : keys) {
				if(ch == 'p') {
					Bytes binary(256000);
					if ( loadBinFile(binary,"/home/lieven/workspace/stm32prog/Blink.bin") ) {
						batchProgram(binary);
						_state = PROGRAMMING;
					}
				} else if(ch == 'b') {
					_state = IDLE;
				} else if(ch == 't') {
					_state = TERMINAL;
				}
			}
		}
	})

	.match(MsgClass::AnyClass,
	[this](Msg& msg) {
		if(_state == PROGRAMMING) {
			uint32_t rc = programming(msg);
			if(rc == PT_EXITED) _state = TERMINAL;
		} else if(_state == TERMINAL) {
			// TODO display data from serial
		}
	})
	/*
		.match(LABEL("getIdReply"),
		[this](Msg& msg) {

		})

		.match(LABEL("getVersionReply"), [this](Msg& msg) {})

		.match(LABEL("getReply"), [this](Msg& msg) {})

		.match(LABEL("readMemoryReply"),
		[this](Msg& msg) {

		})

		.match(LABEL("writeMemoryReply"), [this](Msg& msg) {})

		.match(LABEL("writeUnprotectReply"), [this](Msg& msg) {})

		.match(LABEL("readUnprotectReply"), [this](Msg& msg) {})

		.match(LABEL("eraseAllReply"), [this](Msg& msg) {})

		.match(LABEL("resetFlashReply"), [this](Msg& msg) {})

		.match(LABEL("resetSystemReply"), [this](Msg& msg) {})*/

	.match(MsgClass("pingTimer"), [this](Msg& msg) {
		_stm32.tell(Msg("ping").id(_idCounter++),self());
	})
	.match(MsgClass("pingReply"), [this](Msg& msg) {
		INFO(" PING OK ");
	})

	.match(MsgClass::Properties(), [this](Msg& msg) {sender().tell(replyBuilder(msg)("state",_state==PROGRAMMING ? "programming" : "terminal"),self());})

	.build();
}
