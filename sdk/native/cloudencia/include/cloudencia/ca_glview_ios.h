//
//  sc_glview_ios.h
//  SincityFramework
//
//  Created by Mamadou DIOP on 01/05/15.
//  Copyright (c) 2015 Doubango Telecom. All rights reserved.
//

#ifndef SincityFramework_sc_glview_ios_h
#define SincityFramework_sc_glview_ios_h

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

@protocol SCGlviewIOSDelegate <NSObject>
@optional
-(void) glviewAnimationStarted;
-(void) glviewAnimationStopped;
-(void) glviewVideoSizeChanged;
@end

@interface SCGlviewIOS : UIView {
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    CADisplayLink* _displayLink;
    GLuint _program;
    GLuint _renderBuffer;
    GLuint _framebuffer;
    GLuint _positionSlot;
    GLuint _texCoordSlot;
    GLuint _lumaUniform, _chromaUniformU, _chromaUniformV;
    GLuint _lumaTexture, _chromaTextureU, _chromaTextureV;
    CGFloat _screenWidth, _screenHeight;
    GLint _textureWidth, _textureHeight;
    GLint _viewportX, _viewportY, _viewportW, _viewportH;
    GLint _parNumerator, _parDenominator;
    GLuint _vertexShader, _fragmentShader;
    uint _bufferWidth, _bufferHeight, _bufferSize;
    uint8_t* _buffer;
    GLboolean _animating;
    GLboolean _stopping;
    GLuint _fps;
    GLfloat _aspectRatio;
    id<SCGlviewIOSDelegate> _delegate;
}

-(void)setFps:(GLuint)fps;
-(void)startAnimation;
-(void)stopAnimation;
-(void)setOrientation:(UIDeviceOrientation)orientation;
-(void)setBufferYUV:(const uint8_t*)buffer width:(uint)bufferWidth height:(uint)bufferHeight;
-(void)setDelegate:(id<SCGlviewIOSDelegate>)delegate;
-(void)setPAR:(int)numerator denominator:(int)denominator;
@property(readonly) int viewportX;
@property(readonly) int viewportY;
@property(readonly) int viewportWidth;
@property(readonly) int viewportHeight;
@property(readonly) int videoWidth;
@property(readonly) int videoHeight;
@property(readonly) BOOL animating;
@end

#endif
