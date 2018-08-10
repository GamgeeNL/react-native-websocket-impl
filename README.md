
# react-native-websocket-impl
Enables WebSocket ping and pong messages to be sent from js code.

## Getting started

`$ npm install react-native-websocket-impl --save`

### Mostly automatic installation

`$ react-native link react-native-websocket-impl`

### Manual installation


#### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`
2. Go to `node_modules` ➜ `react-native-websocket-impl` and add `RNWebSocketImpl.xcodeproj`
3. In XCode, in the project navigator, select your project. Add `libRNWebSocketImpl.a` to your project's `Build Phases` ➜ `Link Binary With Libraries`
4. Run your project (`Cmd+R`)<

#### Android

1. Open up `android/app/src/main/java/[...]/MainApplication.java`
  - Add `import com.iskander508.WebSocketImpl.RNWebSocketImplPackage;` to the imports at the top of the file
  - Add `new RNWebSocketImplPackage()` to the list returned by the `getPackages()` method
2. Append the following lines to `android/settings.gradle`:
  	```
  	include ':react-native-websocket-impl'
  	project(':react-native-websocket-impl').projectDir = new File(rootProject.projectDir, 	'../node_modules/react-native-websocket-impl/android')
  	```
3. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
  	```
      compile project(':react-native-websocket-impl')
  	```


## Usage
```javascript
import openConnection from 'react-native-websocket-impl';

openConnection(url, headers)
  .then(socket => {
    console.log('Socket created successfully!')
    socket.send('message')
    socket.ping() // send WS PING frame
    socket.pong() // send WS PONG frame (unsolicited)
  }, err => {
    console.log('Socket creation failed!')
  });
```
  
