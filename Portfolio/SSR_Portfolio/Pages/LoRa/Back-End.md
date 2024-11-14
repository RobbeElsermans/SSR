The server side will provide a place where the data can be stored and then be viewed via graph. For this project docker will be used to set up a:
- MongoDB server:              For data storage.
- Node-Red environment:   To receive the data, save it to the DB and graph it.

# Docker
Using docker we can easily create a separate instance of our back-end on new devices.
To [install docker on Linux](https://docs.docker.com/desktop/install/linux/debian/), take the following steps:
- Install Gnome:
```
sudo apt install gnome-terminal
```
- Install Docker:
```
sudo apt-get update
sudo apt-get install ./docker-desktop-amd64.deb
```
- Check installation:
```
sudo docker --version
```
We will make a Docker compose file to easily reproduce our system.
# MongoDB
The MongoDB instance will be used to store the received data, in case we want to view it on a later date.
The settings of our MongoDB container will be based on the following commands.
```
sudo docker pull mongodb/mongodb-community-server:latest
```
```
sudo docker run --name mongodb -p 27017:27017 -d mongodb/mongodb-community-server:latest
```
# Node-Red
Node-Red is a simple tool, with which we can quickly set up:
- A MQTT receiver
- Dashboard
The settings of our Node-Red container will be based on the following commands.
```
sudo docker pull nodered/node-red:latest
```
```
sudo docker run -it -p 1880:1880 -v node_red_data:/data --name SSR-server nodered/node-red
```
# Docker Compose

[dashboard](https://flows.nodered.org/node/node-red-dashboard)
[MongoDB](https://flows.nodered.org/node/node-red-contrib-mongodb4)
