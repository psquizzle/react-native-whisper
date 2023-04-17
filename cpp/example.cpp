#include <example.h>
#include <louder.h> //Whisper cpp interface
#include <unixify.h>
#include "example.h"
#include <jsi/jsi.h>
#include <thread>
using namespace facebook::jsi;
using namespace std;

namespace example
{

    void install(Runtime &jsiRuntime)
    {

        auto transcribeAudioToText = Function::createFromHostFunction(jsiRuntime,
                                                                    PropNameID::forAscii(jsiRuntime,
                                                                                         "transcribeAudioToText"),
                                                                    0,
                                                                    [](Runtime &runtime,
                                                                       const Value &thisValue,
                                                                       const Value *arguments,
                                                                       size_t arg_count) -> Value
                                                                    {
                                                                        if (arg_count < 1 || !arguments[0].isString())
                                                                        {
                                                                            throw JSError(runtime, "Invalid input to function needs string ()");
                                                                            return {};
                                                                        }
                                                                        string model = arguments[0].asString(runtime).utf8(runtime);
                                                                        string uri = arguments[1].asString(runtime).utf8(runtime);
                                                                        Function callback = arguments[2].getObject(runtime).getFunction(runtime);
                                                                        std::string jsonString = transcribeWavToTextCPP(getUnixPath(model), getUnixPath(uri), std::ref(callback), runtime);
                                                                        auto jsonStr = String::createFromUtf8(runtime, jsonString);

                                                                        return Value();
                                                                    });

        jsiRuntime.global().setProperty(jsiRuntime, "transcribeAudioToText", move(transcribeAudioToText));

         

        auto multiply = Function::createFromHostFunction(jsiRuntime,
                                                         PropNameID::forAscii(jsiRuntime,
                                                                              "multiply"),
                                                         2,
                                                         [](Runtime &runtime,
                                                            const Value &thisValue,
                                                            const Value *arguments,
                                                            size_t count) -> Value
                                                         {
                                                             int x = arguments[0].getNumber();
                                                             int y = arguments[1].getNumber();

                                                             return Value(x * y);
                                                         });

        jsiRuntime.global().setProperty(jsiRuntime, "multiply", move(multiply));

        auto multiplyWithCallback = Function::createFromHostFunction(jsiRuntime,
                                                                     PropNameID::forAscii(jsiRuntime,
                                                                                          "multiplyWithCallback"),
                                                                     3,
                                                                     [](Runtime &runtime,
                                                                        const Value &thisValue,
                                                                        const Value *arguments,
                                                                        size_t count) -> Value
                                                                     {
                                                                         int x = arguments[0].getNumber();
                                                                         int y = arguments[1].getNumber();

                                                                         arguments[2].getObject(runtime).getFunction(runtime).call(runtime, x * y);

                                                                         return Value();
                                                                     });

        jsiRuntime.global().setProperty(jsiRuntime, "multiplyWithCallback", move(multiplyWithCallback));

        auto foo = Function::createFromHostFunction(
            jsiRuntime,
            PropNameID::forAscii(jsiRuntime, "foo"),
            1,
            [](Runtime &runtime, const Value &thisValue, const Value *arguments, size_t count) -> Value
            {
                auto userCallbackRef = std::make_shared<Object>(arguments[0].getObject(runtime));

                auto f = [&runtime](shared_ptr<Object> userCallbackRef)
                {
                    auto val = String::createFromUtf8(runtime, "THIS IS A TEST");
                    auto error = Value::undefined();
                    userCallbackRef->asFunction(runtime).call(runtime, error, val);
                };

                std::thread thread_object(f, userCallbackRef);
                thread_object.detach();

                return Value::undefined();
            });
        jsiRuntime.global().setProperty(jsiRuntime, "foo", std::move(foo));
    }

}
