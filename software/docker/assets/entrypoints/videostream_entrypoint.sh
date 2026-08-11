#!/usr/bin/env bash

#rpicam-vid --camera $CAMERA_ID --low-latency --width 480 --height 360 --codec libav --libav-format h264 -t 0 -o - | gst-launch-1.0 fdsrc fd=0 ! h264parse ! decodebin ! videoconvert ! videoscale ! webrtcsink run-signalling-server=true signalling-server-port=$SIGNALING_PORT
rpicam-vid --camera $CAMERA_ID --low-latency --width 1280 --height 720 --codec libav --libav-format h264 -t 0 -o - | gst-launch-1.0 fdsrc fd=0 ! udpsink host=$UDP_HOST port=$UDP_PORT