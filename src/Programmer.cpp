
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


Programmer::Programmer(ActorRef& keyboard, ActorRef& bridge)
	: _keyboard(keyboard)
	, _bridge(bridge)
	, _stm32("ESP32-12857/programmer", bridge)
	,_globalServices("global/services",bridge)
	, _batch(*new MsgBatch())
	,_binary(1024000) {
	config.setNameSpace("programmer");
	config.get("binFile",_binFile,"./main.bin");
	config.get("programmingBaudrate",_programmingBaudrate,115200);
	config.get("terminalBaudrate",_terminalBaudrate,115200);
	_state = IDLE;
	_idCounter=0;
	_pingReplied=true;
}

Programmer::~Programmer() {}
void Programmer::preStart() {

	_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 10000);
	timers().startPeriodicTimer("",Msg("pingTimer"),3000);
	_stateSelect = &receiveBuilder().match(MsgClass("check"), [this](Msg&) {}).build();

	eb.subscribe(self(), MessageClassifier(_keyboard,Keyboard::keyPressed));
	eb.subscribe(self(), MessageClassifier(_stm32,MsgClass("uart"))); 								//ActorRef needs to be on heap and continue to exist after this call
	eb.subscribe(self(), MessageClassifier(_globalServices, MsgClass("stm32programmer")));

}

Receive& Programmer::createReceive() {
	return receiveBuilder()

	.match(MsgClass("uart"),[this](Msg& msg) {
		std::string str;
		if(msg.get("data", str)==0)
			printf("%s",str.c_str());
	})

	.match(Keyboard::keyPressed,
	[this](Msg& msg) {
		std::string keys;
		if(msg.get("data", keys) == 0) {
			for(char ch : keys) {
				if(ch == 'p') {
					if ( _state==PROGRAMMING) {
						printf("> still busy...\n");
					} else {
						printf("> programming to %s.\n",_stm32.path());
						if ( loadBinFile(_binary,_binFile.c_str()) ) {
							batchProgram(_binary);
							_state = PROGRAMMING;
							PT_INIT(&_pt); // reset state machine
						}
					}
				} else if(ch == 'r') {
					_stm32.tell(Msg("resetFlash")("xx",1),self());
					printf("> reset to flash \n");
				} else if(ch == 's') {
					_stm32.tell(Msg("resetSystem")("xx",1),self());
					printf("> reset to system \n");
				}
			}
		}
	})

	.match(MsgClass::AnyClass,
	[this](Msg& msg) {
		if(_state == PROGRAMMING) {
			uint32_t rc = programming(&_pt,msg); // invoke state machine
			if(rc == PT_EXITED) {
				printf("failed.\n");
				_state=TERMINAL;
			} else if ( rc ==PT_ENDED) {
				printf("running.\n");
				_state=TERMINAL;
			}
		} else if(_state == TERMINAL) {
			// TODO display data from serial
		}
	})

	.match(MsgClass("uart"),
	[this](Msg& msg) {
		std::string str;
		if (msg.get("data",str)==0) {
			fprintf(stdout,"%s",str.c_str());
			fflush(stdout);
		}
	})

	.match(MsgClass("pingTimer"), [this](Msg& msg) {
		if ( _prevPingReplied !=  _pingReplied  ) {
			if ( _pingReplied ) printf("===================== ONLINE =======================\n");
			else printf("===================== OFFLINE ======================\n");
		}
		_stm32.tell(Msg("ping").id(_idCounter++),self());
		_prevPingReplied=_pingReplied;
		_pingReplied=false;
	})

	.match(MsgClass("pingReply"), [this](Msg& msg) {
		_pingReplied=true;
	})

	.match(LABEL("resetFlashReply"), [this](Msg& msg) {
		printf(" reset flash done.\n");
	})

	.match(LABEL("resetSystemReply"), [this](Msg& msg) {
		printf(" reset system done.\n");
	})

	.match(MsgClass::Properties(), [this](Msg& msg) {sender().tell(replyBuilder(msg)("state",_state==PROGRAMMING ? "programming" : "terminal"),self());})

	.build();
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
	_batch.clear();
	_batch.add(Msg("resetSystem")("baudrate",_programmingBaudrate).id(_idCounter++));
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
		sectorData.clear();
		while(binImage.hasData() && sectorData.hasSpace(1))
			sectorData.write(binImage.read());
		while(sectorData.length() % 4 != 0) sectorData.write(0xFF);
		Base64::encode(sectorBase64, sectorData);
		_batch.add(Msg("writeMemory")("addressHex", addressHex)("data", sectorBase64).id(_idCounter++));
	}
	_batch.add(Msg("readMemory")("addressHex", "8000000").id(_idCounter++));
	_batch.add(Msg("resetFlash")("baudrate",_terminalBaudrate).id(_idCounter++));
	_idxBatchSend=0;
	_idxBatchReply=0;
}

uint32_t Programmer::programming(struct pt* pt,Msg& msg) {
	static uint32_t window = 5;

	PT_BEGIN(pt);

	printf("> programming STM32 : %d bytes from file : %s \n",_binary.length(),_binFile.c_str());
	for( _idxBatchSend = 0; _idxBatchSend < window; _idxBatchSend++) { //send first window
		Msg& m=_batch.at(_idxBatchSend);
		_stm32.tell(m,self());
		INFO(" [%d] %s ",m.id(),Label::label(m.cls()));
	}
	_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 3000);

	while(true) {
		PT_YIELD(pt);

		if(msg.cls() == MsgClass("timeout").id()) {
			ERROR(" programming stopped , timeout encountered.");
			PT_EXIT(pt);
		} else if (msg.cls() == replyCls(_batch.at(_idxBatchReply).cls())) {
			timers().cancel(_timer1);
			int erc;
			uid_type id;
			Msg& m = _batch.at(_idxBatchReply);
			if((msg.get("erc", erc) == 0) && ( msg.get(UD_ID,id)==0)) {
				printf(" %d/%d [%d] %s = %d \n",_idxBatchReply,_batch.size()-1,id,Label::label(_batch.at(_idxBatchReply).cls()),erc);
				INFO(" %d/%d [%d] %s = %d \n",_idxBatchReply,_batch.size()-1,id,Label::label(_batch.at(_idxBatchReply).cls()),erc);
				_idxBatchReply++;

				if (erc == E_OK && m.id()==id ) {
					if(_idxBatchReply == _batch.size()) break;
					if(_idxBatchSend  < _batch.size()) {
						Msg& m=_batch.at(_idxBatchSend);
						_stm32.tell(m,self());
						INFO(" [%d] %s ",m.id(),Label::label(m.cls()));
						_idxBatchSend++;
					}
				} else {
					ERROR("stm32programmer returned erc : %d or different id : %d vs %d ",erc,id,m.id());
				}
				_timer1 = timers().startSingleTimer("timeout", Msg("timeout"), 3000);
			} else {
				ERROR(" programming stopped , cannot retrieve erc . %s ",msg.toString().c_str());
				ERROR("%s",((Xdr)msg).toString().c_str());
				PT_EXIT(pt);
			}
		} else {
			WARN(" unexepected message %s , expected %s : ignored ",msg.toString().c_str(),Label::label(replyCls(_batch.at(_idxBatchReply).cls())));
//			PT_EXIT(pt);
		}
	}
	printf(" programming finished.\n");
	PT_END(pt);
}
