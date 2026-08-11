set -e

apt update 
apt-get install -y \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly gstreamer1.0-libav \
    gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa \
    gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 \
    gstreamer1.0-pulseaudio gstreamer1.0-nice

# Install rust to build webrtcsink plugin
export CARGO_HOME=/home/ubuntu/.cargo
export RUSTUP_HOME=/home/ubuntu/.rustup
export PATH="${CARGO_HOME}/bin:${PATH}"
apt install -y curl build-essential gcc
curl --proto '=https' --tlsv1.2 https://sh.rustup.rs -sSf | sh -s -- -y
chmod -R a+rw $CARGO_HOME

# Build the webrtcsink plugin
cd /tmp
apt install -y git-all
git clone https://gitlab.freedesktop.org/gstreamer/gst-plugins-rs.git
cd gst-plugins-rs
git checkout b4450d31e261cf2537a1ed9657e82fed84c075f4
cargo build --release -p gst-plugin-webrtc --lib
mkdir -p /usr/lib/gst-plugins
mv target/release/libgstrswebrtc.so /usr/lib/gst-plugins/
cd /tmp
rm -rf gst-plugins-rs

# Uninstall rust as we no longer need it
yes | rustup self uninstall
