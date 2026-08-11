
Build process
================================

Build
------------------------

.. note::
    Before building, please follow the :doc:`../../hardware/equipments_configuration/user_pc/index` steps

To build the Dragonfly software, simply run the following : 

.. code-block::

    git clone https://github.com/AntoineDiers/Dragonfly.git --recurse-submodules
    cd Dragonfly/software
    ./build.sh

This will generate the following :

- **deploy/dragonfly-hmi_*_amd64.deb** : The installer for the Dragonfly HMI
- **deploy/dragonfly-run.tar** : A tar file containing the dragonfly-run docker image
- **deploy/docker-compose.yaml** : The docker-compose file that will start the docker image on Dragonfly's Flight Controller
- The **dragonfly-run** docker image that will run on Dragonfly's Flight Controller


Deploy
------------------------

Deploy DragonFly HMI
_______________________________

.. code-block::

    sudo dpkg -i deploy/dragonfly-hmi_*.deb

You can then run it with 

.. code-block::

    dragonfly-hmi

Deploy Dragonfly Firmware
_______________________________
    
- Copy the **docker-compose.yaml** file into your raspberry pi

.. code-block::

    scp deploy/docker-compose.yaml dragonfly@dragonfly_wifi:/home/dragonfly

- Ssh into your raspberry_pi
- Pull the docker from your local registry

.. code-block::

    docker pull dev_pc:5000/dragonfly-run
    docker tag dev_pc:5000/dragonfly-run dragonfly-run
 
- Start the dragonfly software

.. code-block::

    cd && docker compose down && docker compose up -d