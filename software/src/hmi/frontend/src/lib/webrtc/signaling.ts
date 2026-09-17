export enum SignalingMessageType
{
  welcome = "welcome",
  peerStatusChanged = "peerStatusChanged",
  list = "list",
  listConsumers = "listConsumers",
  sessionStarted = "sessionStarted",
  peer = "peer",
  startSession = "startSession",
  endSession = "endSession",
  error = "error"
};

export interface SignalingWelcomeMsg
{
    peerId : string;
}

export interface SignalingPeerStatusChangedMsg
{
    peerId : string,
    roles : string[]
}

export interface SignalingListEntry
{
    id : string
}

export interface SignalingListMsg
{
    producers : SignalingListEntry[]
}

export interface SignalingSessionStartedMsg
{
    peerId : string,
    sessionId : string
}