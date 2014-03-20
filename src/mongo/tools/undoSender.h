#ifndef UndoSender_H
#define UndoSender_H

#include "undo_database.pb.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


using namespace std;
using namespace google::protobuf::io;

class UndoSender{
	private:
		UndoProto::DataAccessList* dataAcessList;
		char* host_name;
		int port;
	public:
		UndoSender(const char* hostname,int hostport);
		UndoProto::DataAccessUnit* newRequest(string id,string source, vector<string> readIDs, vector<string> writeIDs);
		int createSocket(char* hostname,int hostport);
		void sendToServer();
};


#endif