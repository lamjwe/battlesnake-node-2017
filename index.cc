// addon.cc
#include <node.h>
#include <string>
#include <iostream>
#include <sstream>
#include "heuristics.h"
namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

enum Direction { NONE=0, UP, DOWN, RIGHT, LEFT, DIE };

// This is the implementation of the "add" method
// Input arguments are passed using the
// const FunctionCallbackInfo<Value>& args struct
void CPPAddon(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  // FROM http://stackoverflow.com/questions/16613828/how-to-convert-stdstring-to-v8s-localstring#16639079
  v8::String::Utf8Value value(args[0]->ToString());
  std::string out_input = std::string(*value);
  
  std::cout << out_input << std::endl;
  Direction d = (Direction)run(out_input);
  // Direction d = DOWN;

  switch (d) {
	case UP:
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "up"));
		break;
	case DOWN:
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "down"));
		break;
	case RIGHT:
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "right"));
		break;
	case LEFT:
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "left"));
		break;
	default:
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "up"));
	}
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "cppaddon", CPPAddon);
}

NODE_MODULE(addon, Init)

}  // namespace demo