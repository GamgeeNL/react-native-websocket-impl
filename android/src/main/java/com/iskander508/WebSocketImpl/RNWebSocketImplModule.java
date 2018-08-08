package com.iskander508.WebSocketImpl;

import com.facebook.react.uimanager.*;
import com.facebook.react.bridge.*;
import com.facebook.react.modules.core.DeviceEventManagerModule;

import java.util.Map;
import java.util.HashMap;

import android.util.Log;

import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;
import org.java_websocket.framing.PongFrame;

import java.net.URI;

public class RNWebSocketImplModule extends ReactContextBaseJavaModule {

    private int counter = 0;
    private final Map<Integer, WebSocketClient> connections = new HashMap<Integer, WebSocketClient>();
    private final ReactApplicationContext reactContext;

    //Constructor
    public RNWebSocketImplModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
        counter = 1;
    }

    //Name for module register to use:
    @Override
    public String getName() {
        return "WebSocketImpl";
    }

    // Open new WebSocket connection
    @ReactMethod
    public void open(String url, ReadableMap headers, Callback successCallback, Callback errorCallback) {
        try {
            final int index;
            synchronized(this) {
                index = this.counter++;
            }

            final Map<String,String> httpHeaders = new HashMap<String,String>();
            for(Map.Entry<String, Object> entry: headers.toHashMap().entrySet()) {
                httpHeaders.put(entry.getKey(), entry.getValue().toString());
            }

            final WebSocketClient client = new WebSocketClient(new URI(url), httpHeaders) {
                @Override
                public void onOpen(ServerHandshake serverHandshake) {
                    Log.i("Websocket", "Opened");
                    final WritableNativeMap params = new WritableNativeMap();
                    params.putInt("index", index);
                    emit("ws-impl-open", params);
                }

                @Override
                public void onMessage(String message) {
                    Log.i("Websocket", "Message " + message);
                    final WritableNativeMap params = new WritableNativeMap();
                    params.putInt("index", index);
                    params.putString("message", message);
                    emit("ws-impl-message", params);
                }

                @Override
                public void onClose(int code, String reason, boolean remote) {
                    Log.i("Websocket", "Closed " + reason);
                    final WritableNativeMap params = new WritableNativeMap();
                    params.putInt("index", index);
                    params.putInt("code", code);
                    params.putString("reason", reason);
                    emit("ws-impl-close", params);
                }

                @Override
                public void onError(Exception e) {
                    Log.i("Websocket", "Error " + e.getMessage());
                    final WritableNativeMap params = new WritableNativeMap();
                    params.putInt("index", index);
                    params.putString("message", e.getMessage());
                    emit("ws-impl-error", params);
                }
            };
            client.connect();

            synchronized(this) {
                this.connections.put(index, client);
            }
            successCallback.invoke(index);
        } catch (Exception e) {
            Log.w("Websocket", "Open " + e.getMessage());
            errorCallback.invoke(e.getMessage());
            return;
        }
    }

    @ReactMethod
    public void close(int index, int code, String reason) {
        try {
            WebSocketClient client;
            synchronized(this) {
                client = this.connections.get(index);
                this.connections.remove(index);
            }
            client.close(code, reason);
        } catch (Exception e) {
            Log.w("Websocket", "Close " + e.getMessage());
        }
    }

    @ReactMethod
    public void send(int index, String message) {
        try {
            WebSocketClient client;
            synchronized(this) {
                client = this.connections.get(index);
            }
            client.send(message);
        } catch (Exception e) {
            Log.w("Websocket", "Send " + e.getMessage());
        }
    }

    @ReactMethod
    public void ping(int index) {
        try {
            WebSocketClient client;
            synchronized(this) {
                client = this.connections.get(index);
            }
            client.sendPing();
        } catch (Exception e) {
            Log.w("Websocket", "Ping " + e.getMessage());
        }
    }

    @ReactMethod
    public void pong(int index) {
        try {
            WebSocketClient client;
            synchronized(this) {
                client = this.connections.get(index);
            }
            client.sendFrame(new PongFrame());
        } catch (Exception e) {
            Log.w("Websocket", "Pong " + e.getMessage());
        }
    }

    void emit(String eventName, WritableMap params) {
        reactContext
            .getJSModule(DeviceEventManagerModule.RCTDeviceEventEmitter.class)
            .emit(eventName, params);
    }
}
