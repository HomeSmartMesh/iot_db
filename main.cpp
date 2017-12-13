/*
The MIT License (MIT)

Copyright (c) 2017 Wassim Filali

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

___________________________________________________________________________________
 dependencies :
 - Poco,Boost filesystem : using The Boost Software License 1.0
 - bme_280 : with datasheet's associated rights see bme280_server.hpp
 - json.hpp MIT License Copyright (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.

___________________________________________________________________________________

 start date : 20.01.2017
 takeover to iot_db : 13.12.2017

 webserver for managing
 * HTTP server
 * HTTP update as a client
 * Websocket server to broadcast sensors data
 * Websocket client to retreive data from other Servers
 
 
*/


//for usleep() sleep()
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>
//for file
#include <fstream>
//for abs
#include <cmath>

//for exit()
#include <cstdlib>


#include "utils.hpp"

#include "mqtt.hpp"

#include "webserver.hpp"

#include <assert.h>
#include <string>
#include <memory>

#include "db_mgr.hpp"

#include "json.hpp"
using json = nlohmann::json;

#include "log.hpp"

#include <boost/filesystem.hpp>
//using namespace boost::filesystem;

using namespace std;

void handle_nodesinfo(std::string &request,json &nodesinfo,std::string &response)
{
	std::cout << "handle_config> request>" << request << std::endl;
	json jReq = json::parse(request);//double parsing on getRequestType and here
	json jResp;
	try
	{
		jResp["response"]["id"] = jReq["request"]["id"];
		jResp["response"]["type"] = "nodesinfo";
		jResp["response"]["nodesinfo"] = nodesinfo;
		response = jResp.dump();
	}
	catch(const std::exception& ex)
	{
		std::cout << "handle_nodesinfo> !!! Caught exception \"" << ex.what() << "\"!!!\n";
	}
	std::cout << "handle_nodesinfo> response length: " << response.length() << std::endl;
}

json read_json(std::string const &filename)
{
	if (boost::filesystem::exists(filename))
	{
		std::cout << "________________________________________________________________"<< std::endl;
		std::cout << "Loading : "<< filename << std::endl;
	}
	else
	{
		std::cout << "File not found : "<< filename << std::endl;
		exit(1);
	}
	std::ifstream data_file(filename);
	json result;
	data_file >> result;
	return result;
}

int main(int argc, const char *argv[]) 
{
	boost::filesystem::path app_path(argv[0]);

	json config = read_json(app_path.parent_path().string()+"/mesh_config/iot_db_config.json");

	json calib = read_json(app_path.parent_path().string()+"/mesh_config/bme280_calibration.json");

	json nodesinfo = read_json(app_path.parent_path().string()+"/mesh_config/nodes.json");

	Log::config(config["log"]);
	webserver_c		wbs(config);	//websocket manager : broadcast() and respond()
	
	Serial 			stream(config["serial"],calib);	// - process serial port stream : - calibrate sensors values
									// - provides ready to store measures MAP of Nodes.Sensors.Values,Timestamp
									// - If not configured to be used then the .update() polling is neutral

	mqtt_c			mqtt(config["mqtt_client"],stream);	//MQTT client app wrapper, will attempt connection on creation if params provided

	db_manager_c	dbm(config["database"]);	//adds values to files and memory db, answers requests
	
	dbm.load();
	
	//#2 issue, it is likely that someone else is using the port in parallel
	//discard first trash buffer if available right after opening the port
	//this discard measure is not enough as ibberish appears still
	
	stream.update();
	
	std::cout << "______________________Main Loop______________________" << std::endl;
	while (1) 
	{
		if(stream.update())
		{
			NodeMap_t measures = stream.processBuffer();
			if(measures.size() != 0)
			{
				localActions(measures,wbs);
				dbm.addMeasures(measures);	//save into the data base (memory db & files db)
				std::string jMeasures = utl::stringify(measures,"update");//data type is "update"
				wbs.broadcast(jMeasures);
				std::string jMeasures2 = utl::stringify2(measures,"update");//data type is "update"
				wbs.post(jMeasures2);//for another webserver if configured
				mqtt.publish_measures(measures);
			}
		}

		//run() contains the loop needed to process certain QoS messages and reconnect if connection lost
		mqtt.run();
		
		//5 ms : this is an unnneccessary load if the processing grows up
		usleep(5000);
		
		std::string request = wbs.poll();
		
		if(!request.empty())
		{
			std::string response;
			std::string requestType = utl::getRequestType(request);
			if(requestType.find("Duration") == 0)
			{
				dbm.handle_duration(request,response);
			}
			else if(requestType.find("update") == 0)
			{
				dbm.handle_update(request,response);
			}
			else if(requestType.find("nodesinfo") == 0)
			{
				handle_nodesinfo(request,nodesinfo,response);
			}
			else
			{
				std::cout << "main> Error : undefined request type : " << requestType<<std::endl;
			}

			if(!response.empty())
			{
				wbs.respond(response);
			}
		}
		
	}

	return 0;
}
