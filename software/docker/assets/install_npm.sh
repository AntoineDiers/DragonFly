apt update 
apt install -y curl
mkdir -p $NVM_DIR
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash
. $NVM_DIR/nvm.sh
nvm install $NODE_VERSION