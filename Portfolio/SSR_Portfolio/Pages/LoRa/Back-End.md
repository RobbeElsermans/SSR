The server side will provide a place where the data can be stored and then be viewed via graph. For this project docker will be used to set up a:
- MongoDB server:              For data storage.
- Node-Red environment:   To receive the data, save it to the DB and graph it.
# Installations
## Docker
Using docker we can easily create a separate instance of our back-end on new devices.
To [install docker on Linux](https://docs.docker.com/desktop/install/linux/debian/), take the following steps:
- Install Gnome:
``` bash
sudo apt install gnome-terminal
```
- Install Docker:
``` bash
sudo apt-get update
sudo apt-get install ./docker-desktop-amd64.deb
```
- Check installation:
``` bash
sudo docker --version
```
We will make a Docker compose file to easily reproduce our system.
## Base container installations
### MongoDB
The MongoDB instance will be used to store the received data, in case we want to view it on a later date.
The settings of our MongoDB container will be based on the following commands.
``` bash
sudo docker pull mongodb/mongodb-community-server:latest
```
``` bash
sudo docker run --name mongodb -p 27017:27017 -d mongodb/mongodb-community-server:latest
```
The MongoDB terminal can be installed to check the data.
``` bash
sudo apt-get install gnupg curl
```
``` bash
curl -fsSL https://www.mongodb.org/static/pgp/server-8.0.asc | \
   sudo gpg -o /usr/share/keyrings/mongodb-server-8.0.gpg \
   --dearmor
```
``` bash
echo "deb [ signed-by=/usr/share/keyrings/mongodb-server-8.0.gpg ] http://repo.mongodb.org/apt/debian bookworm/mongodb-org/8.0 main" | sudo tee /etc/apt/sources.list.d/mongodb-org-8.0.list
```
``` bash
sudo apt-get update
```
### Node-Red
Node-Red is a simple tool, with which we can quickly set up:
- A MQTT receiver
- Dashboard
The settings of our Node-Red container will be based on the following commands.
``` bash
sudo docker pull nodered/node-red:latest
```
``` bash
sudo docker run -it -p 1880:1880 -v node_red_data:/data --name SSR-server nodered/node-red
```
## Docker compose
Using the following Docker compose file we can create both a MongoDB and Node-Red container that are capable of interacting with each other.
``` yaml
version: '1.0'

services:
  node-red:
    image: nodered/node-red				# Node-RED Docker image
    container_name: SSR-server				# Set the container name
    restart: always
    ports:
      - "1880:1880"					# Expose port 1880 for Node-RED
    volumes:
      - node_red_data:/data				# Persist data in a Docker volume
    networks:
      - app-network					# Attach to custom network
    environment:
      - MONGO_URL=mongodb://mongodb:27017		# MongoDB connection URL (use service name as host)
    entrypoint:
      - /bin/sh						# Use shell to execute commands
      - -c
      - |
        npm stop;					# Stop Node-RED
        npm install node-red-dashboard;			# Install dashboard nodes
        npm install node-red-contrib-mongodb4;		# Install MongoDB nodes
        npm start;                             		# Retart Node-RED
    # Enter the container with the following command:	sudo docker exec -it <container_id> bash

  mongodb:
    image: mongodb/mongodb-community-server:latest	# MongoDB Docker image
    container_name: SSR-DB				# Set container name for MongoDB
    restart: always
    ports:
      - "27017:27017"					# Expose MongoDB port
    volumes:
      - mongo_data:/data/db				# Persist MongoDB data in a Docker volume
    networks:
      - app-network					# Attach to custom network
    # environment:
      # MONGO_INITDB_ROOT_USERNAME: root
      # MONGO_INITDB_ROOT_PASSWORD: root

volumes:
  node_red_data:					# Volume for Node-RED data
  mongo_data:						# Volume for MongoDB data

networks:
  app-network:						# Define network for communication between containers
```
*Note that we will automatically install **the node-red-dashboard** & **node-red-contrib-mongodb4 packages**.*

# Back-end setup
## LoRa connectivity
### MQTT node
Get a "mqtt in" node and put it in the flow, then click on it. A window should slide open. Click on the plus button next to the "server" to setup a connection to TheThingsNetwork.
![[mqtt_server_creation.png]]
Setup server connection.
![[mqtt_server_connection_settings.png]]
Setup server connection security.
![[mqtt_server_security_settings.png]]
Subscribe to a certain topic. *For testing porpusus we can use '#' as a any-topic*
![[mqtt_settings.png]]
### Function node
Get a "function" node and put it in the flow, then click on it. A window should slide open.
In the "OnMessage" tab drop the following code:
``` js
msg.payload = msg.payload.uplink_message.decoded_payload.bytes
//at+msg="hello world"
return msg;
```
### Debug node
Add last drop a "debug" node and put it in the flow, and connect:
1) The MQTT output to the function input.
2) The function output to the debug intput.
![[mqtt_debug_flow.png]]
## Container connectivity
Add the following nodes to the flow:
- An **inject** node
- 2 **function** nodes
- A **[mongodb4](https://flows.nodered.org/node/node-red-contrib-mongodb4)**  node
- A **debug** node
Then connect them as follows:
![[mongodb_flow.png]]
### Function nodes
Click on the "function" node. A window should slide open.
First let's make a find all function using the following code:
``` js
msg = {};
return msg;
```
Then let's make a find by ID function using the following code:
``` js
msg.payload = {hostname: "73e5846ccdd6"};
// 73e5846ccdd6 is example id found via the "Find all" node
return msg;
```
### MongoDB node
Click on the "mongodb4" node. A window should slide open.
Create a new connection:
![[mongodb_connection_creation.png]]
Set the simple URI settings:
![[mongodb_simple_uri_settings.png]]
Set the advanced URI settings:
![[mongodb_advanced_uri_settings.png]]
Set the global settings:
![[mongodb_settings.png]]
## Dashboard
[dashboard nodes](https://flows.nodered.org/node/node-red-dashboard)
