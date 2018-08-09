import {NativeModules, DeviceEventEmitter, NativeEventEmitter, Platform} from 'react-native';
import EventEmitter from 'eventemitter3';

const {WebSocketImpl} = NativeModules;

const ReadyState = {
  CONNECTING: 0,
  OPEN: 1,
  CLOSING: 2,
  CLOSED: 3
};

// currently existing sockets, index => socket
const Sockets = {};
let iOSCounter = 1;

const onOpen = socket => {
  socket.readyState = ReadyState.OPEN;
  socket.emitter.emit('open');
};
const onMessage = (socket, message) => socket.emitter.emit('message', message);
const onClose = (socket, code, reason) => {
  socket.readyState = ReadyState.CLOSED;
  delete Sockets[socket.index];
  socket.index = null;
  socket.emitter.emit('close', code, reason);
};
const onError = (socket, message) => socket.emitter.emit('error', message);

const Emitter = Platform.select({
  android: DeviceEventEmitter,
  ios: new NativeEventEmitter(WebSocketImpl)
});
const listener = handler => evt => {
  if (Sockets[evt.index]) {
    handler(Sockets[evt.index], evt);
  }
};
Emitter.addListener('ws-impl-open', listener(onOpen));
Emitter.addListener(
  'ws-impl-message',
  listener((socket, {message}) => onMessage(socket, message))
);
Emitter.addListener(
  'ws-impl-close',
  listener((socket, {code, reason}) => onClose(socket, code, reason))
);
Emitter.addListener(
  'ws-impl-error',
  listener((socket, {message}) => onError(socket, message))
);

export default function openConnection(url, headers = {}) {
  const emitter = new EventEmitter();
  const socket = {
    ...ReadyState,
    emitter,
    index: undefined,
    readyState: ReadyState.CONNECTING,
    on: (type, handler) => emitter.on(type, handler),
    off: (type, handler) => emitter.off(type, handler),
    send: message => socket.index && WebSocketImpl.send(socket.index, message),
    ping: () => socket.index && WebSocketImpl.ping(socket.index),
    pong: () => socket.index && WebSocketImpl.pong(socket.index),
    close: (code = 1000, reason = '') => {
      if (socket.index) {
        socket.readyState = ReadyState.CLOSED;
        WebSocketImpl.close(socket.index, code, reason);
        socket.index = null;
      }
    }
  };

  if (Platform.OS === 'android') {
    return new Promise((resolve, reject) =>
      WebSocketImpl.open(
        url,
        headers,
        index => {
          socket.index = index;
          Sockets[index] = socket;
          resolve(socket);
        },
        message => reject(new Error(message))
      )
    );
  } else {
    socket.index = iOSCounter++;
    WebSocketImpl.connect(
      url,
      null,
      {headers},
      socket.index
    );
    Sockets[socket.index] = socket;
    return Promise.resolve(socket);
  }
}
