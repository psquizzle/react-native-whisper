# React Native Whisper

This is a proof of concept of a JSI library for [OpenAI Whisper ](https://openai.com/research/whisper), it combines a C++ port of Whisper from [whisper.cpp](https://github.com/ggerganov/whisper.cpp) and a JSI template from [this blog.](https://blog.notesnook.com/getting-started-react-native-jsi/)


## Usage 

```
import Whisper from 'react-native-whisper'

Whisper.transcribeWavToText(pathOfModel,pathOfAudio,(text)=>{alert(text)})

```

You can specify the location of the model or .wav audio file by providing either its full path (e.g., file:///path/to/file) or its Unix-style path (e.g., /path/to/file).




This project is in no way stable or complete!! Feel free to contribute.


## Installation

```sh
npm install react-native-whisper
```

```sh
yarn add react-native-whisper
```

For iOS also run `pod install` in `/ios` folder.

If you are using with Expo, run

```sh
expo prebuild
```


## Prerequisites
You must have Android NDK and CMake installed on android to build the library.

## Methods
The following methods are implemented.

### Platfom agnostic
The relevant code is present in `cpp/example.cpp` file.
```ts
    helloWorld():string;

    multiplyWithCallback(x:number,y:number,callback:(z:number) => void):void
    
    multiply(x:number,y:number):number
```

### Platform specific
The relevant code on android is in `android/cpp-adapter.cpp` and `ios/SimpleJsi.mm` on iOS.
```ts
    getDeviceName():string
    
    setItem(key:string,value:string):boolean

    getItem(key:string):string
```

## Basic usage example
```tsx
import simpleJsiModule from "react-native-jsi-template";

simpleJsiModule.helloWorld() // returns helloworld.
```
Run the example app for more.


## Thanks to these libraries & their authors:
The initial work done by authors of the following libraries has helped a lot in writing the blog and keeping this repo updated.

- [react-native-mmkv](https://github.com/mrousavy/react-native-mmkv/)
- [react-native-reanimated](https://github.com/software-mansion/react-native-reanimated/)

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT
