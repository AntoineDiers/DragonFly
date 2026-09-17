import { SignalingMessageType, SignalingWelcomeMsg, SignalingPeerStatusChangedMsg, SignalingListMsg, SignalingSessionStartedMsg } from './signaling'

export class WebRtcClient
{
    _video : HTMLVideoElement;
    _signaling_ws : WebSocket;
    _own_peer_id : string = "";
    _session_id : string = "";
    _ready : boolean = false;
    _rtc_connection : RTCPeerConnection;
    _bytes_received : number = 0;

    constructor(video : HTMLVideoElement,  signaling_ws_url : string)
    {
        this._video = video;   
        this._signaling_ws = new WebSocket(signaling_ws_url);
        this._rtc_connection = new RTCPeerConnection(
        {
            iceServers: [
            {
                urls: [
                "stun:stun.l.google.com:19302",
                "stun:stun1.l.google.com:19302"
                ]
            }
            ],
            bundlePolicy: "max-bundle"
        });

        this._signaling_ws.onerror = (event : Event) => { this.onWsError(event); };
        this._signaling_ws.onclose = () => { this.onWsClose(); };
        this._signaling_ws.onmessage = (event : MessageEvent) => { this.onWsMessage(event); };

        this._rtc_connection.ontrack = (event : RTCTrackEvent) => { this.onRtcTrack(event); }

        setInterval(() => { this.updateStatus(); }, 1000);
    }

    updateStatus()
    {
        this._video.play().catch((ex) => {}); 

        this._rtc_connection.getStats().then((stats : RTCStatsReport) => 
        {
            stats.forEach(k => {
                if (k.type === "inbound-rtp" && k.kind === "video") 
                {
                    if (this._bytes_received === k.bytesReceived) 
                    {
                        this._rtc_connection.close();
                    }
                    this._bytes_received = k.bytesReceived;
                }
            })

        },
        (error : any) => 
        {
            console.log("Error when getting rtc stats : ", error);
            this._rtc_connection.close();
        });
    }

    isStreamAlive() : boolean
    {
        return this._rtc_connection.connectionState == 'connected';
    }

    // -------------------------------------------------
    //          Signaling Websocket callbacks
    // -------------------------------------------------

    onWsError(event : Event)
    {
        console.log("Websocket error : ", event);
        this._signaling_ws.close();
        this._rtc_connection.close();
    }

    onWsClose()
    {
        console.log("Websocket connection was closed");
        this._rtc_connection.close();
    }

    onWsMessage(event : MessageEvent)
    {
        try 
        {
            const msg : any = JSON.parse(event.data);
            console.log("Received websocket message : ", msg);
            switch(msg.type)
            {
                case SignalingMessageType.welcome :             this.onSignalingWelcome(msg); break;
                case SignalingMessageType.peerStatusChanged :   this.onSignalingPeerStatusChanged(msg); break;
                case SignalingMessageType.list :                this.onSignalingList(msg); break;
                case SignalingMessageType.sessionStarted :      this.onSignalingSessionStarted(msg); break;
                case SignalingMessageType.peer :                this.onSignalingPeer(msg); break;
            }
        }
        catch(ex)
        {
            console.log("Error while processing websocket message : ", ex);
            this._rtc_connection.close();
        }
    }

    onSignalingWelcome(msg : SignalingWelcomeMsg)
    {
        console.log("Got welcome message : ", msg);
        this._own_peer_id = msg.peerId;
        this._signaling_ws.send(JSON.stringify(
        {
            type: "setPeerStatus",
            roles: ["listener"],
            meta: null 
        }));
    }

    onSignalingPeerStatusChanged(msg : SignalingPeerStatusChangedMsg)
    {
        console.log("Got PeerStatusChanged message : ", msg);
        if(msg.peerId == this._own_peer_id)
        {   
            if(!this._ready && msg.roles.includes("listener"))
            {
                this._ready = true;
                this._signaling_ws.send(JSON.stringify({ type: "list" }));
            }
        }
        else
        {
            if(msg.roles.includes("producer"))
            {
                console.log("New producer");
            }
        }
    }

    onSignalingList(msg : SignalingListMsg)
    {
        console.log("Got Producers list : ", msg);
        if(msg.producers.length == 0)
        {
            console.log("No producers found");
            this._rtc_connection.close();
        }

        this._signaling_ws.send(JSON.stringify({
            type: "startSession",
            peerId: msg.producers[0].id
        }));
    }

    onSignalingSessionStarted(msg : SignalingSessionStartedMsg)
    {
        this._session_id = msg.sessionId;
    }

    onSignalingPeer(msg : any)
    {
        if(msg.ice)
        {
            console.log("Got ice candidate : ", msg.ice);
            this._rtc_connection.addIceCandidate(new RTCIceCandidate(msg.ice));
        }
        if(msg.sdp)
        {
            this._rtc_connection.setRemoteDescription(msg.sdp)
            .then(() => 
            {
                return this._rtc_connection.createAnswer();
            })
            .then((desc : any) => 
            {
                return desc ? this._rtc_connection.setLocalDescription(desc) : null;
            })
            .then(() => 
            {
                console.log("Sending SDP with session id", this._session_id);
                this._signaling_ws.send(JSON.stringify(
                {
                    type: "peer",
                    sessionId: this._session_id,
                    sdp: this._rtc_connection.localDescription!.toJSON()
                }));
            });
        }
    }

    // -------------------------------------------------
    //          RTC Connection callbacks
    // -------------------------------------------------

    onRtcTrack(event : RTCTrackEvent)
    {
        console.log("Got RTC track : ", event);
        if (event.streams.length > 0) 
        {
            this._video.srcObject = event.streams[0];
        }
        else
        {
            this._rtc_connection.close();
        }
    }
};