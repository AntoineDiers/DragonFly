#!/usr/bin/env bash

gst-launch-1.0 udpsrc port=$UDP_PORT ! h264parse ! decodebin ! videoconvert ! videoscale ! webrtcsink run-signalling-server=true signalling-server-port=$SIGNALING_PORT