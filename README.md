# IoT db Comparision with existing solutions
- Mongodb with Node.js proxy for front end
- redis based solution (to be developed)
- influxDB "update" : actually [influxDB](https://docs.influxdata.com/influxdb/v1.4/introduction/getting_started/) as namely a time series database, is the reason why this custom db project is canceled

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

### Environment and Dependencies
- Scons : a single line for the whole compilation, linker stuf,...
- C++11 : Modern c++ is easier than javascript (the luxury of having a compiler)
- Boost 1.60 (filesystem) : A warm welcome to Boost on RPI (see install instructions)
- Poco-1.7.7 : HTTP server&client, websocket server : https://pocoproject.org/
- json for modern c++ : json as easy to use as in javascript https://github.com/nlohmann/json (to be replaced by Poco::JSON)
