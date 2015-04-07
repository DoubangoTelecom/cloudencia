/**
@fileoverview This is Cloudencia WebRTC "library".

@name        libcloudencia
@author      Doubango Telecom
@version     1.0.0
*/
if (!window.cloudencia) {
    cloudencia = { debug: true };
}
if (window.cloudencia.debug) {
    document.write(unescape("%3Cscript src='cloudencia-adapter.js' type='text/javascript'%3E%3C/script%3E"));
    document.write(unescape("%3Cscript src='cloudencia-base64.js' type='text/javascript'%3E%3C/script%3E"));
    document.write(unescape("%3Cscript src='cloudencia-md5.js' type='text/javascript'%3E%3C/script%3E"));
}
var WebSocket = (window['MozWebSocket'] || window['MozWebSocket'] || WebSocket);
window.console = window.console || {};
window.console.info = window.console.info || window.console.log || function(msg) { };
window.console.error = window.console.error || window.console.log || window.console.info;
if (!!((Object.getOwnPropertyDescriptor && Object.getOwnPropertyDescriptor(window, "ActiveXObject")) || ("ActiveXObject" in window))) { // Internet Explorer?
    // CustomEvent
    (function () {
        function CustomEvent(event, params) {
            params = params || { bubbles: false, cancelable: false, detail: undefined };
            var evt = document.createEvent('CustomEvent');
            evt.initCustomEvent(event, params.bubbles, params.cancelable, params.detail);
            return evt;
        };

        CustomEvent.prototype = window.Event.prototype;
        window.CustomEvent = CustomEvent;
    })();
}

/**
@namespace Anonymous
@description Engine
*/
var CAEngine = {
    /** @private */socket: null,
    /** @private */config: {
        "user_id": null,
        "user_password": null,
    },
    /** @private */connected: false,
    /** @private */ready: false,

    /* ===================================
    CONSTATNTS 
    ===================================== */
    WEBSOCKET_PROTOCOL: "cloudencia-signaling",

    PREFIX_CALLID_AUTH: "@auth@",
    PREFIX_CALLID_CHAT: "@chat@",
    PREFIX_CALLID_CALL: "@call@",
    PREFIX_CALLID_PRESENCE: "@presence@",

    EVENT_TYPE_SIGNALING: "signaling",
    EVENT_TYPE_CALL: "call",
    EVENT_TYPE_CHAT: "chat",
};
/**
@namespace Anonymous
@description Message types
*/
var CA_MSG_TYPE = {
    UNKNOWN: "unknown",
    ERROR: "error",
    SUCCESS: "success",
    PROVISIONAL: "provisional",
    AUTHCONN: "authConn",
    CHAT: "chat",
    OFFER: "offer",
    ANSWER: "answer", // Call-Event
    PRANSWER: "pranswer", // Call-Event
    HANGUP: "hangup", // Call-Event
};
/**
@namespace Anonymous
@description SDP types
*/
var CA_SDP_TYPE = {
    OFFER: "offer",
    ANSWER: "answer",
    PRANSWER: "pranswer",
};

/**
@namespace Anonymous
@description Content types
*/
var CA_CONTENT_TYPE = {
    UNKNOWN: "data/unknown",
    TEXT: "data/text",
    HTML: "data/html",
    BASE64: "data/base64", // will need subtype
};

/**
@namespace Anonymous
@description Events types
*/
var CA_EVENT_TYPE = {
    SIGNALING: {
        ERROR: "signaling-error",
        CONNECTING: "signaling-connecting",
        CONNECTED: "signaling-connected",
        READY: "signaling-ready",
        DISCONNECTING: "signaling-disconnecting",
        DISCONNECTED: "signaling-disconnected",
    },
    CALL: {

    },
    CHAT: {
        RESULT_TRANSAC: "chat-result-transac"
    },
};

/**
@namespace Anonymous
@description Response code types
*/
var CA_RESPONSE_CODE_TYPE = {
    PROVISIONAL: 100, // [100 - 199]
    SUCCESS: 200, // [200 - 299]
    ERROR: 300, // [300 - [
};

/**
@namespace Anonymous
@description Response codes
*/
var CA_RESPONSE_CODE = {
    PROVISIONAL: {
        TRYING: 100,
        RINGING: 101
    },
    SUCCESS: {
        OK: 200,
        SENT: 201,
        DELIVERED: 202,
        SEEN: 203,
    },
    ERROR: {
        NOK: 300,
        FORBIDDEN: 301,
        NOTFOUD: 302,
        NOTSENT: 303
    }
};

/**
@namespace Anonymous
@description Call session
*/
var CACall = function (){
    /* from: null,
    to: null,
    cid: null,
    tid: null,
    pc: null,
    config: null,
    localStream: null,
    video_remote_elt: null,
    pendingOffer: null,
    latestRemoteDescription: FF clears unknown attributes in  "pc.remoteDescription" */
    this.config = 
    { 
        audio_send: false, 
        audio_recv: false,
        audio_remote_elt: null,
        video_send: false,
        video_recv: true,
        video_local_elt: null,
        video_remote_elt: null
    };
};


/**
Sets the call config.
@param {CAConfigCall} config Call config
*/
CACall.prototype.setConfig = function(config) {
    if (config) {    
        this.config.audio_send = (config.audio_send !== undefined) ? config.audio_send : false;
        this.config.audio_recv = (config.audio_recv !== undefined) ? config.audio_recv : false;
        this.config.audio_remote_elt = (config.audio_remote_elt || CAEngine.config.audio_remote_elt);
        this.config.video_send = (config.video_send !== undefined) ? config.video_send : false;
        this.config.video_recv = (config.video_recv !== undefined) ? config.video_recv : true;
        this.config.video_local_elt = (config.video_local_elt || CAEngine.config.video_local_elt);
        this.config.video_remote_elt = (config.video_remote_elt || CAEngine.config.video_remote_elt);
    }
}

/**
Makes an outgoing call.
@returns {Boolean} <i>true</i> if succeed; otherwise <i>false</i>
@throws {ERR_NOT_CONNECTED}
@throws {ERR_NOT_ALREADY_INCALL}
*/
CACall.prototype.call = function() {
    if (!CAEngine.connected) {
        throw new Error("not connected"); 
    }
    if (this.pc) {
        throw new Error("already in call"); 
    }
    
    var This = this;
    if (this.config.audio_send || this.config.video_send) {
        console.info("getUserMedia(audio:" + this.config.audio_send + ", video:" + this.config.video_send + ")");
        getUserMedia(
                CAUtils.buildConstraintsGUM(this.config), 
                function(stream) { 
                    console.info("getUserMedia OK");
                    CAWebRtcEvents.ongotstream(This, stream);
                    CAUtils.makeOffer(This); // send offer
                },
                function (err) { 
                    console.error("getUserMedia:" + err);
                    CAUtils.raiseCallEvent(This, "error", err);
                }
            );
    }
    else {
        CAUtils.makeOffer(This); // send offer
    }
    return true;
}

/**
Accepts an incoming message received from the signaling server.
@param {CAMessage} message The message to accept.
@throws {ERR_NOT_CONNECTED}
@throws {ERR_NOT_NOT_IMPLEMENTED}
*/
CACall.prototype.acceptMsg = function(msg) {
    if (!CAEngine.connected) {
        throw new Error("not connected");
    }
    var This = this;
    if (msg.type === CA_SDP_TYPE.ANSWER || msg.type === CA_SDP_TYPE.PRANSWER) {
        var sdpRemote = msg.sdp.replace(/UDP\/TLS\/RTP\/SAVPF/g, 'RTP/SAVPF');
        this.pc.setRemoteDescription(new RTCSessionDescription({ type: msg.type, sdp: sdpRemote }),
            function() {
                console.info("setRemoteDescription OK");
            },
            function(err) {
                console.error("setRemoteDescription NOK:" + err);
                CAUtils.raiseCallEvent(This, "error", err);
            }
        );
    }
    else if (msg.type === CA_SDP_TYPE.OFFER) {
        var This = this;
        var Msg = msg;
        if (this.config.audio_send || this.config.video_send) {
            console.info("getUserMedia(audio:" + this.config.audio_send + ", video:" + this.config.video_send + ")");
            getUserMedia(
                CAUtils.buildConstraintsGUM(this.config),
                function(stream) {
                    console.info("getUserMedia OK");
                    CAWebRtcEvents.ongotstream(This, stream);
                    CAUtils.makeAnswer(This, Msg); // send answer
                },
                function(err) {
                    console.error("getUserMedia:" + err);
                    CAUtils.raiseCallEvent(This, "error", err);
                }
            );
        }
        else {
            CAUtils.makeAnswer(This, Msg); // send answer
        }
    }
    else if (msg.type === CA_MSG_TYPE.HANGUP) {
        CAUtils.closeMedia(This);
    }
    else {
        throw new Error("not implemented yet");
    }
    return true;
}
/**
Rejects an incoming message received from the signaling server.
@param {CAMessage} message The message to reject.
@throws {ERR_NOT_CONNECTED}
*/
CACall.rejectMsg = function(msg) {
    if (!CAEngine.connected) {
        throw new Error("not connected"); 
    }
    var msg_text = JSON.stringify({
        from: msg.to,
        to: msg.from,
        cid: msg.cid,
        tid: msg.tid,
        type: CA_MSG_TYPE.HANGUP
    });
    console.info("send: " + msg_text);
    CAEngine.socket.send(msg_text);
    return true;
}
/**
Terminates the call session.
@throws {ERR_NOT_CONNECTED}
*/
CACall.prototype.hangup = function() {
    CAUtils.closeMedia(this);
    if (!CAEngine.connected) {
        throw new Error("not connected"); 
    }
    var msg_text = JSON.stringify({
        from: this.from,
        to: this.to,
        cid: this.cid,
        tid: CAUtils.stringRandomUuid(),
        type: CA_MSG_TYPE.HANGUP
    });
    console.info("send: " + msg_text);
    CAEngine.socket.send(msg_text);
    return true;
}

/**
Builds a new <a href="CACall.html">CACall</a> object.
@param {String / CAMessage} dest Could be a <b>String</b> or a <a href="CAMessage.html">CAMessage</a> object with <a href="CAMessage.html#type">type</a> equal to "offer".
@param {CAConfigCall} [config] Call config <br />
The <b>String</b> verstion is for outgoing calls while the <a href="CAMessage.html">CAMessage</a> version is for incoming calls.
@returns {CACall} new call session.
*/
CACall.build = function(dest, config) {
    var newCall;
    if (typeof dest == "string") { 
        newCall = new CACall();
        newCall.from = CAEngine.config.localId;
        newCall.to = dest;
        newCall.cid = CAUtils.stringRandomUuid();
        newCall.tid = CAUtils.stringRandomUuid();
    }
    else if (dest.type == CA_MSG_TYPE.OFFER){
        newCall = new CACall();
        newCall.from = dest.to;
        newCall.to = dest.from;
        newCall.cid = dest.cid;
        newCall.tid = dest.tid;
        newCall.ticket = dest.ticket;
    }
    if (newCall && config) {
        newCall.setConfig(config);
    }
    return newCall;
}

/** @private */
var CAWebRtcEvents = {
    onicecandidate: function(call, e) {
        var This = call;
        if (e.candidate) {
            if (false) { // intentionally disabled
                call.pc.addIceCandidate(new RTCIceCandidate(e.candidate),
                    function () { console.info("addIceCandidate OK"); },
                    function (err) {
                        console.error("addIceCandidate NOK:" + err);
                        CAUtils.raiseCallEvent(This, "error", err);
                    }
                );
            }
        }
        if (!e.candidate || (call.pc && (call.pc.iceState === "completed" || call.pc.iceGatheringState === "completed"))) {
            if (!call.pc) {
                // closing the peerconnection could raise this event with null candidate --> ignore
                console.info("ICE gathering done...but pc is null");
                return;
            }
            console.info("ICE gathering done");
            CAUtils.raiseCallEvent(This, "info", "gathering ICE candidates done!");
            if (!CAEngine.connected) {
                CAUtils.raiseCallEvent(This, "error", "ICE gathering done but signaling transport not connected");
                return;
            }
            var msg = {
                from: call.from,
                to: call.to,
                cid: call.cid,
                tid: call.tid,
                type: call.pc.localDescription.type, // "offer"/"answer"/"pranswer"
                sdp: CAUtils.stringLocalSdp(call)
            };
            var msg_text = JSON.stringify(msg);
            console.info("send: " + msg_text);
            CAEngine.socket.send(msg_text);
        }
    },
    onaddstream: function(call, e) { // remote stream
        CAUtils.attachStream(call, e.stream, false/*remote*/);
        CAUtils.raiseCallEvent(call, "info", "media started!");
    },
    ongotstream: function(call, stream) { // local stream
        call.localStream = stream;
        if (call.pc) {
            call.pc.addStream(stream);
            CAUtils.attachStream(call, stream, true/*local*/);
        }
    }
};

/** @private */
var CAUtils = {
    stringFormat: function(s_str) {
        for (var i = 1; i < arguments.length; i++) {
            var regexp = new RegExp('\\{' + (i - 1) + '\\}', 'gi');
            s_str = s_str.replace(regexp, arguments[i]);
        }
        return s_str;
    },
    stringRandomFromDict: function(i_length, s_dict) {
        var s_ret = "";
        for (var i = 0; i < i_length; i++) {
            s_ret += s_dict[Math.floor(Math.random() * s_dict.length)];
        }
        return s_ret;
    },
    stringRandom: function(i_length) {
        var s_dict = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXTZabcdefghiklmnopqrstuvwxyz";
        return CAUtils.stringRandomFromDict(i_length, s_dict);
    },
    stringRandomUuid: function() {
        // e.g. 6ba7b810-9dad-11d1-80b4-00c04fd430c8
        var s_dict = "0123456789abcdef";
        return CAUtils.stringFormat("{0}-{1}-{2}-{3}-{4}",
                CAUtils.stringRandomFromDict(8, s_dict),
                CAUtils.stringRandomFromDict(4, s_dict),
                CAUtils.stringRandomFromDict(4, s_dict),
                CAUtils.stringRandomFromDict(4, s_dict),
                CAUtils.stringRandomFromDict(12, s_dict));
    },
    stringLocalSdp: function(call) {
        if (call.pc && call.pc.localDescription && call.pc.localDescription.sdp) {
            var sdp = call.pc.localDescription.sdp.replace(/RTP\/SAVPF/g, 'UDP/TLS/RTP/SAVPF');
            var arrayPosOfVideo = [];
            var posOfVideo, indexOfVideoStart, indexOfVideo, indexOfVideoNext, indexOfContentSlides;
            // Find video positions to consider as "screencast"
            if (call.latestRemoteDescription && call.latestRemoteDescription.sdp) {
                posOfVideo = 0;
                indexOfVideoStart = 0;
                while ((indexOfVideo = call.latestRemoteDescription.sdp.indexOf("m=video ", indexOfVideoStart)) > 0) {
                    if ((indexOfContentSlides = call.latestRemoteDescription.sdp.indexOf("a=content:slides", indexOfVideo)) > 0 || (indexOfContentSlides = call.latestRemoteDescription.sdp.indexOf("label:doubango@bfcpvideo", indexOfVideo)) > 0) {
                        indexOfVideoNext = call.latestRemoteDescription.sdp.indexOf("m=video ", indexOfVideo + 1);
                        if (indexOfVideoNext == -1 || indexOfContentSlides < indexOfVideoNext) {
                            // Video at "posOfVideo" is "screencast"
                            arrayPosOfVideo.push(posOfVideo);
                        }
                    }
                    indexOfVideoStart = indexOfVideo + 1;
                    ++posOfVideo;
                }
            }
            // Patch the SDP to have it looks like "screencast"
            for (var i = 0; i < arrayPosOfVideo.length; ++i) {
                posOfVideo = 0;
                indexOfVideoStart = 0;
                while ((indexOfVideo = sdp.indexOf("m=video ", indexOfVideoStart)) > 0) {
                    if (posOfVideo == arrayPosOfVideo[i]) {
                        var endOfLine = sdp.indexOf("\r\n", indexOfVideo);
                        if (endOfLine > 0) {
                            sdp = sdp.substring(0, (endOfLine + 2)) + "a=content:slides\r\n" + sdp.substring((endOfLine + 2), sdp.length);
                        }
                    }
                    indexOfVideoStart = indexOfVideo + 1;
                    ++posOfVideo;
                }
            }

            return sdp;
        }
    },
    buildPeerConnConfig: function() {
        return {
            iceServers: CAEngine.config.iceServers
        };
    },
    buildConstraintsSDP: function(callConfig) {
        return {
            'mandatory': {
                'OfferToReceiveAudio': (callConfig && callConfig.audio_recv !== undefined) ? callConfig.audio_recv : false,
                'OfferToReceiveVideo': (callConfig && callConfig.video_recv !== undefined) ? callConfig.video_recv : true
            }
        };
    },
    buildConstraintsGUM: function(callConfig) {
        return {
            'audio': (callConfig && callConfig.audio_send),
            'video': (callConfig && callConfig.video_send)
        };
    },
    buildAuthToken: function (email, password) {
        // auth-token = md5('password' ':' 'login' ':' 'doubango.org');
        return CAMD5.hexdigest(password + ':' + email + ':' + 'doubango.org');
    },
    buildHa1: function (impi, realm, password) {
        /* RFC 2617 - 3.2.2.2 A1
                    A1       = unq(username-value) ":" unq(realm-value) ":" passwd
            */
        return CAMD5.hexdigest(impi + ':' + realm + ':' + password);
    },
    buildResult: function (code) {
        return { code: code }; // "CAResult"
    },
    buildResultTransac: function(code, cid, tid) {
        return { code: code, cid: cid, tid: tid }; // CAResultTransac
    },
    attachStream: function(call, stream, local) {
        var htmlElementVideo = local ? (call.config.video_local_elt || CAEngine.config.video_local_elt) : (call.config.video_remote_elt || CAEngine.config.video_remote_elt);
        var htmlElementAudio = local ? (call.config.localAudio || CAEngine.config.localAudio) : (call.config.audio_remote_elt || CAEngine.config.audio_remote_elt);

        if (htmlElementAudio) {
            attachMediaStream(htmlElementAudio, stream);
        }
        if (htmlElementVideo) {
            attachMediaStream(htmlElementVideo, stream);
        }
    },
    closeMedia: function(call) {
        if (call) {
            if (call.pc) {
                if (call.localStream) {
                    try { call.pc.removeStream(call.localStream); } catch (e) { }
                }
                call.pc.close();
                call.pc = null;
            }
            if (call.localStream) {
                call.localStream.stop();
                call.localStream = null;
            }

            CAUtils.attachStream(call, null, false/*remote*/);
            CAUtils.attachStream(call, null, true/*local*/);
        }
    },
    makeOffer: function(call) {
        var This = call;
        if (!call.pc) {
            call.pc = new RTCPeerConnection(CAUtils.buildPeerConnConfig());
            if (call.localStream) {
                call.pc.addStream(call.localStream);
                CAUtils.attachStream(call, call.localStream, true/*local*/);
            }
        }
        call.pc.onicecandidate = function(e) { CAWebRtcEvents.onicecandidate(This, e); }
        call.pc.onaddstream = function(e) { CAWebRtcEvents.onaddstream(This, e); }
        call.pc.createOffer(
            function(desc) {
                console.info("createOffer:" + desc.sdp);
                This.pc.setLocalDescription(desc,
                    function() {
                        console.info("setLocalDescription OK");
                        CAUtils.raiseCallEvent(This, "info", "gathering ICE candidates...");
                    },
                    function(err) {
                        console.error("setLocalDescription:" + err);
                        CAUtils.raiseCallEvent(This, "error", err);
                    });
            },
            function(err) {
                console.error("createOffer:" + err);
                CAUtils.raiseCallEvent(This, "error", err);
            },
            CAUtils.buildConstraintsSDP(This.config));
    },
    makeAnswer: function(call, Msg) {
        var This = call;
        if (!call.pc) {
            call.pc = new RTCPeerConnection(CAUtils.buildPeerConnConfig());
            if (call.localStream) {
                call.pc.addStream(call.localStream);
                CAUtils.attachStream(call, call.localStream, true/*local*/);
            }
        }
        call.pc.onicecandidate = function(e) { CAWebRtcEvents.onicecandidate(This, e); }
        call.pc.onaddstream = function(e) { CAWebRtcEvents.onaddstream(This, e); }
        var sdpRemote = new RTCSessionDescription({ type: CA_MSG_TYPE.OFFER, sdp: Msg.sdp.replace(/UDP\/TLS\/RTP\/SAVPF/g, 'RTP/SAVPF') });
        call.pc.setRemoteDescription(sdpRemote,
            function() {
                console.info("setRemoteDescription OK");
                This.latestRemoteDescription = sdpRemote;
                This.pc.createAnswer(
                    function(desc) {
                        console.info("createAnswer:" + desc.sdp);
                        This.pc.setLocalDescription(desc,
                            function() {
                                console.info("setLocalDescription OK");
                                CAUtils.raiseCallEvent(This, "info", "gathering ICE candidates...");
                            },
                            function(err) {
                                console.error("setLocalDescription:" + err);
                                CAUtils.raiseCallEvent(This, "error", err);
                            });
                    },
                    function(err) {
                        console.info("createAnswer:" + err);
                        CAUtils.raiseCallEvent(This, "error", err);
                    },
                    CAUtils.buildConstraintsSDP(This.config));
            },
            function(err) {
                console.info("setRemoteDescription NOK:" + err);
                CAUtils.raiseCallEvent(This, "error", err);
            }
        );
    },
    raiseSignalingEvent: function (type, description) {
        if (document != null) {
            // second argument for CustomEvent is CAEventSignaling object
            document.dispatchEvent(new CustomEvent(CAEngine.EVENT_TYPE_SIGNALING, { 'detail': { type: type, description: description } }));
        }
    },
    raiseCallEvent: function(call, type, description, msg) {
        if (CAEngine.oncall) {
            CAEngine.oncall({ "type": type, "call": call, "description": description, "msg": msg });
        }
    },
    sendData: function (data) {
        if (!CAEngine.socket || !CAEngine.connected) {
            throw new Error("not connected");
        }
        if (data) {
            console.info("sendData: " + data);
            CAEngine.socket.send(data);
        }
    }
};

/** @private */
var CASignaling = {
    connectionAuthenticated: false,
    msgAuthConnection: null,
    sendAuthConnection: function () {
        if (CAEngine.connected) {
            CASignaling.msgAuthConnection = {
                type: CA_MSG_TYPE.AUTHCONN,
                from: CAEngine.config.user_id,
                cid: CASignaling.stringRandomId(CAEngine.PREFIX_CALLID_AUTH),
                tid: CASignaling.stringRandomId(CAEngine.PREFIX_CALLID_AUTH),
                authToken: CAUtils.buildAuthToken(CAEngine.config.user_id, CAEngine.config.user_password)
            };
            var JSONText = JSON.stringify(CASignaling.msgAuthConnection);
            CAUtils.sendData(JSONText);
        }
    },
    sendResponse: function(request, type, code) {
        var JSONText = JSON.stringify({
            type: type,
            code: code,
            from: CAEngine.config.user_id,
            to: request.from,
            cid: request.cid,
            tid: request.tid,
            ticket: request.ticket,
            authToken: CASignaling.currnetAuthToken()
        });
        CAUtils.sendData(JSONText);
    },
    sendIM: function (to/*String: email*/, content/*Any data supporting toString()*/, contentType/*String: optional*/, contentSubType/*String: optional*/) {
        // returns CAResultTransac
        if (!CAEngine.socket || !CAEngine.ready) {
            throw new Error("not ready");
        }
        if (!to || !content) {
            throw new Error("Invalid input");
        }
        var JSONObj = {
            type: CA_MSG_TYPE.CHAT,
            from: CAEngine.config.user_id,
            to: to,
            cid: CASignaling.stringRandomId(CAEngine.PREFIX_CALLID_CHAT),
            tid: CASignaling.stringRandomId(CAEngine.PREFIX_CALLID_CHAT),
            content: { data: content, type: contentType ? contentType : CA_CONTENT_TYPE.TEXT, subType: contentSubType },
            authToken: CASignaling.currnetAuthToken()
        };
        CAUtils.sendData(JSON.stringify(JSONObj));
        return CAUtils.buildResultTransac(CA_RESPONSE_CODE.SUCCESS.SENT, JSONObj.cid, JSONObj.tid);
    },
    currnetAuthToken: function() {
        return CASignaling.msgAuthConnection ? CASignaling.msgAuthConnection.authToken : CAUtils.buildAuthToken(CAEngine.config.user_id, CAEngine.config.user_password);
    },
    isResponseFor: function(resp, req) {
        return req && resp && req.cid === resp.cid && req.tid === resp.tid;
    },
    stringRandomId: function (prefix) {
        return (prefix + (new Date()).getTime() + "@" + CAUtils.stringRandomUuid());
    }
};

/**
Initialize the engine. Must be the first function to call.
@param {CAConfigEngine} config
*/
CAEngine.init = function(config) {
    CAEngine.config = config;
}

/**
Connects to the signaling server. Must be called before making any call.
@param {String} url A WebSocket URL.e.g. "wss://localhost:9000/WebSocket"
@throws {ERR_ALREADY_CONNECTED}
*/
CAEngine.connect = function(url) {
    console.info("connecting to: " + url);
    if (CAEngine.socket) {
        throw new Error("already connected"); 
    }
    
    CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.CONNECTING);
    CAEngine.socket = new WebSocket(url, CAEngine.WEBSOCKET_PROTOCOL);
    CAEngine.socket.onopen = function (e) {
        console.info("signaling::onopen");
        CAEngine.connected = true;
        CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.CONNECTED, (e || {}).data);
        // authenticate the newly connected connection
        CAEngine.ready = false;
        CASignaling.sendAuthConnection();
    }
    CAEngine.socket.onclose = function (e) {
        console.info("signaling::onclose");
        CAEngine.connected = false;
        CAEngine.ready = false;
        CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.DISCONNECTED, (e || {}).data);
        CAEngine.socket = null;        
    }
    CAEngine.socket.onerror = function (e) {
        console.info("signaling::onmessage: " + (e || {}).data);
        CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.ERROR, (e || {}).data);
    }
    CAEngine.socket.onmessage = function(e) {
        console.info("signaling::onmessage: " + e.data);
        var msg = JSON.parse(e.data);
        if (msg) {
            if (msg.type == CA_MSG_TYPE.SUCCESS) {
                if (CASignaling.isResponseFor(msg, CASignaling.msgAuthConnection)) {
                    CAEngine.ready = true;
                    CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.READY);
                }
            }
           else if (msg.type == CA_MSG_TYPE.ERROR) {
                if (CASignaling.isResponseFor(msg, CASignaling.msgAuthConnection)) {
                    CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.ERROR, msg.reason);
                }
           }
           else if (msg.type == CA_MSG_TYPE.CHAT) {
               CASignaling.sendResponse(msg, CA_MSG_TYPE.SUCCESS, CA_RESPONSE_CODE.SUCCESS.DELIVERED);
           }
           else if (msg.type === CA_MSG_TYPE.OFFER || msg.type === CA_MSG_TYPE.ANSWER || msg.type == CA_MSG_TYPE.PRANSWER || msg.type == CA_MSG_TYPE.HANGUP) {
               CAUtils.raiseCallEvent(null, msg.type, msg.type, msg);
           }
        }
    }
    return true;
}

/**
Disconnect the engine from the signaling server.
*/
CAEngine.disconnect = function() {
    if (CAEngine.socket) {
        CAUtils.raiseSignalingEvent(CA_EVENT_TYPE.SIGNALING.DISCONNECTING);
        CAEngine.socket.close();
        CAEngine.socket = null;
    }
    return true;
}

/**
@param {String} to
@param {Object} content
@param {String} [contentType]
@param {String} [contentSubType]
@returns {CAResultTransac}
*/
CAEngine.sendIM = function (to, content, contentType, contentSubType) {
    return CASignaling.sendIM(to, content, contentType, contentSubType);
}


/**
JSON message from the server.
@namespace Anonymous
@name CAMessage
@property {String} type The message type. Possible values: "offer", "answer", "pranswer" or "hangup". <b>Required</b>.
@property {String} to The destination identifer. <b>Required</b>.
@property {String} from The source identifier. <b>Required</b>.
@property {String} cid The call (dialog) id. <b>Required</b>.
@property {String} tid The transaction id. <b>Required</b>.
@property {String} sdp The session description. <i>Optional</i>.
*/


/**
Event object.
@namespace Anonymous
@name CAEventSignaling
@property {String} type
@property {String} description
*/

/**
Event object.
@namespace Anonymous
@name CAEventCall
@property {String} type The event type. Possible values: "offer", "answer", "pranswer", "hangup", "error" or "info".
@property {CACall} call The call object associated to this event.
@property {String} description The event description.
@property {CAMessage} msg The message associated to this event.
*/

/**
Engine configuration object.
@namespace Anonymous
@name CAConfigEngine
@property {String} user_id Local user identifier (most probably an email address).
@property {String} user_password Local user password.
@property {HTMLVideoElement} [video_local_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLVideoElement">HTMLVideoElement<a> where to display the local video stream.
@property {HTMLVideoElement} [video_remote_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLVideoElement">HTMLVideoElement<a> where to display the remote video stream.
@property {HTMLAudioElement} [audio_remote_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLAudioElement">HTMLAudioElement<a> used to playback the remote audio stream.
@property {Array} iceServers Array of STUN/TURN servers to use. The format is as explained at <a href="http://www.w3.org/TR/webrtc/#rtciceserver-type" target=_blank>http://www.w3.org/TR/webrtc/#rtciceserver-type</a> <br />
Example: [{ url: 'stun:stun.l.google.com:19302'}, { url:'turn:user@numb.viagenie.ca', credential:'myPassword'}] 
*/

/**
Call configuration object.
@namespace Anonymous
@name CAConfigCall
@property {Boolean} [audio_send] Defines whether to send audio. Default value: <b>false</b>.
@property {Boolean} [audio_recv] Defines whether to accept incoming audio. Default value: <b>false</b>.
@property {HTMLAudioElement} [audio_remote_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLAudioElement">HTMLAudioElement<a> used to playback the remote audio stream.
@property {Boolean} [video_send] Defines whether to send video. Default value: <b>false</b>.
@property {Boolean} [video_recv] Defines whether to accept incoming video.  Default value: <b>true</b>.
@property {HTMLVideoElement} [video_local_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLVideoElement">HTMLVideoElement<a> where to display the local video stream.
@property {HTMLVideoElement} [video_remote_elt] <a href="https://developer.mozilla.org/en-US/docs/DOM/HTMLVideoElement">HTMLVideoElement<a> where to display the remote video stream.<br />
Example: { audio_send: true, audio_recv: true, video_send: false, video_recv: true }
*/

/**
Result.
@namespace Anonymous
@name CAResult
@property {Short} code
*/

/**
Transaction result.
@namespace Anonymous
@name CAResultTransac
@extends CAResult
@property {String} cid
@property {String} tid
*/
