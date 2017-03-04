// addon.cc
#include <node.h>
#include <string>
#include <iostream>

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

// This is the implementation of the "add" method
// Input arguments are passed using the
// const FunctionCallbackInfo<Value>& args struct
void CPPAddon(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  // Check the number of arguments passed.
  if (args.Length() < 1) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  // // Check the argument types
  // if (!args[0]->IsNumber()) {
  //   isolate->ThrowException(Exception::TypeError(
  //       String::NewFromUtf8(isolate, "Wrong arguments")));
  //   return;
  // }

  // Perform the operation
  // char* value = args[0]->ToString();
  // Local<Number> num = Number::New(isolate, value); // Local<string>?

  // FROM http://stackoverflow.com/questions/16613828/how-to-convert-stdstring-to-v8s-localstring#16639079
  v8::String::Utf8Value value(args[0]->ToString());
  char out_input[] = std::string(*value);

  // Set the return value (using the passed in
  // FunctionCallbackInfo<Value>&)
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, out_input));
}

void Init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "cppaddon", CPPAddon);
}

NODE_MODULE(addon, Init)

}  // namespace demo