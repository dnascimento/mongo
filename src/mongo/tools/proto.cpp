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
using namespace UndoProto;
using namespace google::protobuf::io;

//TODO: create DataAccessList to add
DataAccessList* dataAcessList = new DataAccessList();
string hostname;
int port;

DataAccessUnit* newRequest(string id,string source, vector<string> readIDs, vector<string> writeIDs){
	//add to dataAcessList
	DataAccessUnit* msg = dataAcessList->add_msgs();
	msg->set_id(id);
	msg->set_source(source);
	for(int i = 0; i < readIDs.size(); i++){
		msg->add_readids(readIDs[i]);
	}
	for(int i = 0; i < writeIDs.size(); i++){
		msg->add_writeids(writeIDs[i]);
	}
	return msg;
}



int createSocket(string hostname,int host_port){
	const char* host_name = hostname.c_str();
	int hsock;
	struct sockaddr_in my_addr;
	hsock = socket(AF_INET,SOCK_STREAM,0);
	if(hsock < 0){
		cerr << "error creating socket";
		return -1;
	}
	int err;
	int* p_int;
    p_int = (int*)malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
            (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
            printf("Error setting options %d\n",errno);
            free(p_int);	
            return -1;
    }
    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr(host_name);
    if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
            if((err = errno) != EINPROGRESS){
                    fprintf(stderr, "Error connecting socket %d\n", errno);
                    return -1;
            }
    }
    return hsock;
}

void send(){

	DataAccessList listToSend = *dataAcessList;
	dataAcessList = new DataAccessList();

	int socket = createSocket(hostname,port);
	if(socket == -1){
		cerr << "send: Error creating socket";
		return;
	}
	int siz = listToSend.ByteSize();
	unsigned char buffer[siz];

	ArrayOutputStream* arr = new ArrayOutputStream(buffer,siz);
	CodedOutputStream* output = new CodedOutputStream(arr);
	listToSend.SerializeToCodedStream(output);

	delete output;
	delete arr;
	if((send(socket, (char *) buffer,siz,0))== -1 ) {
		fprintf(stderr, "Error sending data %d\n", errno);
	}

	close(socket);
}

int main(){
	vector<string> readIDs;
	readIDs.push_back("read1");
	readIDs.push_back("read2");

	vector<string> writeIDs;
	writeIDs.push_back("write1");
	writeIDs.push_back("write2");
	DataAccessUnit* request = newRequest("id","sourceServer",readIDs,writeIDs);
	DataAccessUnit* request2 = newRequest("id2","sourceServer2",readIDs,writeIDs);

	send("127.0.0.1",9090);
}