//
//   Copyright 2012 Square Inc.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#import <Foundation/Foundation.h>
#import <Security/SecCertificate.h>

typedef NS_ENUM(unsigned int, RCTWSImplReadyState) {
    RCTWSImpl_CONNECTING   = 0,
    RCTWSImpl_OPEN         = 1,
    RCTWSImpl_CLOSING      = 2,
    RCTWSImpl_CLOSED       = 3,
};

typedef NS_ENUM(NSInteger, RCTWSImplStatusCode) {
    RCTWSImplStatusCodeNormal = 1000,
    RCTWSImplStatusCodeGoingAway = 1001,
    RCTWSImplStatusCodeProtocolError = 1002,
    RCTWSImplStatusCodeUnhandledType = 1003,
    // 1004 reserved.
    RCTWSImplStatusNoStatusReceived = 1005,
    // 1004-1006 reserved.
    RCTWSImplStatusCodeInvalidUTF8 = 1007,
    RCTWSImplStatusCodePolicyViolated = 1008,
    RCTWSImplStatusCodeMessageTooBig = 1009,
};

@class RCTWSImplWebSocket;

extern NSString *const RCTWSImplWebSocketErrorDomain;
extern NSString *const RCTWSImplHTTPResponseErrorKey;

#pragma mark - RCTWSImplWebSocketDelegate

@protocol RCTWSImplWebSocketDelegate;

#pragma mark - RCTWSImplWebSocket

@interface RCTWSImplWebSocket : NSObject <NSStreamDelegate>

@property (nonatomic, weak) id<RCTWSImplWebSocketDelegate> delegate;

@property (nonatomic, readonly) RCTWSImplReadyState readyState;
@property (nonatomic, readonly, strong) NSURL *url;

// This returns the negotiated protocol.
// It will be nil until after the handshake completes.
@property (nonatomic, readonly, copy) NSString *protocol;

// Protocols should be an array of strings that turn into Sec-WebSocket-Protocol.
- (instancetype)initWithURLRequest:(NSURLRequest *)request protocols:(NSArray<NSString *> *)protocols NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithURLRequest:(NSURLRequest *)request;

// Some helper constructors.
- (instancetype)initWithURL:(NSURL *)url protocols:(NSArray<NSString *> *)protocols;
- (instancetype)initWithURL:(NSURL *)url;

// Delegate queue will be dispatch_main_queue by default.
// You cannot set both OperationQueue and dispatch_queue.
- (void)setDelegateOperationQueue:(NSOperationQueue *)queue;
- (void)setDelegateDispatchQueue:(dispatch_queue_t)queue;

// By default, it will schedule itself on +[NSRunLoop RCTWSImpl_networkRunLoop] using defaultModes.
- (void)scheduleInRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)unscheduleFromRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;

// RCTWSImplWebSockets are intended for one-time-use only.  Open should be called once and only once.
- (void)open;

- (void)close;
- (void)closeWithCode:(NSInteger)code reason:(NSString *)reason;

// Send a UTF8 String or Data.
- (void)send:(id)data;

// Send Data (can be nil) in a ping message.
- (void)sendPing:(NSData *)data;

// Send Data (can be nil) in a pong message.
- (void)sendPong:(NSData *)data;

@end

#pragma mark - RCTWSImplWebSocketDelegate

@protocol RCTWSImplWebSocketDelegate <NSObject>

// message will either be an NSString if the server is using text
// or NSData if the server is using binary.
- (void)webSocket:(RCTWSImplWebSocket *)webSocket didReceiveMessage:(id)message;

@optional

- (void)webSocketDidOpen:(RCTWSImplWebSocket *)webSocket;
- (void)webSocket:(RCTWSImplWebSocket *)webSocket didFailWithError:(NSError *)error;
- (void)webSocket:(RCTWSImplWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean;
- (void)webSocket:(RCTWSImplWebSocket *)webSocket didReceivePong:(NSData *)pongPayload;

@end

#pragma mark - NSURLRequest (CertificateAdditions)

@interface NSURLRequest (CertificateAdditions)

@property (nonatomic, readonly, copy) NSArray *RCTWSImpl_SSLPinnedCertificates;

@end

#pragma mark - NSMutableURLRequest (CertificateAdditions)

@interface NSMutableURLRequest (CertificateAdditions)

@property (nonatomic, copy) NSArray *RCTWSImpl_SSLPinnedCertificates;

@end

#pragma mark - NSRunLoop (RCTWSImplWebSocket)

@interface NSRunLoop (RCTWSImplWebSocket)

+ (NSRunLoop *)RCTWSImpl_networkRunLoop;

@end
