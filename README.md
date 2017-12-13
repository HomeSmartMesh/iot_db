# IoT DB
Q : Seriously another database ? Why ?
A : simply because with all the integration efforts of existing solutions requirements are not close to be met. What if it is much easier to implement those requirements ?

# Comparision with existing solutions
I tried to use existing solutions, currently 3 options are available target is to have them up all the time for benchmarking:
- Mongodb with Node.js proxy for front end
- redis based solution (to be developed)
- iot_db (this project)

# Features
## Home Smart Mesh application
years of hundreds of sensors with user's data under user's control, fog based not cloud based approach, so no target to scale up to billions of devices. Fitting into a raspberry pi maximising tinkering possibilities.
## Performance, performance and performance
### Websocket interface
Easy and direct access to Front-end client, no duplication of steps going through another server other than the database itself.
### Column-Store Structure
Targetting sensors data which are time series, a Column-Store is more fit for quicker management.
### Simple and predictable Cache
All the data is cached, and the websocket client has direct access to it, no cache miss since the first request, no complicated cache management. This is of course at the cost of the RAM size limit, but this project does not pretend to scale higher than the fog home user's sensors use case, so why make the db slow and unpredictable for a use case that is not required ?
Basicall, the micro-steps from the front end websocket request to the websocket response are minimal.
## Filesystem persistance simplicity
The persistance is not the bottleneck here as data has to fit in RAM first, which leave room for a relaxed and user friendly persistance format.
It is here text based and structured into files for every sensor and month of data. Users can open files directly in text editors or in excel. Thus no admin client is required.

#C, Cpp clients
##Install
```bash
apt-get install libmosquitto-dev
sudo apt-get install libmosquittopp-dev
```
