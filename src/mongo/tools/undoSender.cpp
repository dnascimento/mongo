#include "undoSender.h"

using namespace std;
using namespace UndoProto;
using namespace google::protobuf::io;


UndoSender::UndoSender(const char* hostname,int hostport){
		host_name = new char[strlen(hostname)];
		strcpy(host_name,hostname);
		port = hostport;
		dataAcessList = new DataAccessList();
}

DataAccessUnit* UndoSender::newRequest(string id,string source, vector<string> readIDs, vector<string> writeIDs){
	//add to dataAcessList
	DataAccessUnit* msg = dataAcessList->add_msgs();
	msg->set_id(id);
	msg->set_source(source);
	for(unsigned long i = 0; i < readIDs.size(); i++){
		msg->add_readids(readIDs[i]);
	}
	for(unsigned long i = 0; i < writeIDs.size(); i++){
		msg->add_writeids(writeIDs[i]);
	}
	return msg;
}

int UndoSender::createSocket(char* host_name,int host_port){
	int hsock;
	struct sockaddr_in my_addr;
	hsock = socket(AF_INET,SOCK_STREAM,0);
	if(hsock < 0){
		cerr << "error creating socket";
		return -1;
	}
	int* p_int;
    p_int = (int*)malloc(sizeof(int));
    *p_int = 1;

    if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
       	 (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        	cerr << "Error setting options\n";
       		free(p_int);	
        	return -1;
    }
    free(p_int);

    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(host_port);

    memset(&(my_addr.sin_zero), 0, 8);
    my_addr.sin_addr.s_addr = inet_addr(host_name);
    if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
       cerr << "Error connecting socket\n";
       return -1;
    }
    return hsock;
}

void UndoSender::sendToServer(){

	DataAccessList listToSend = *dataAcessList;
	dataAcessList = new DataAccessList();
	cout << host_name
		 << "\n";
    cout << port 
    	 << "\n";
	int socket = createSocket(host_name,port);
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
		cerr << "Error sending data\n";
	}
	cout << "sent";
	close	(socket);
}



// int main(){
// 	const char* host = "127.0.0.1";
// 	UndoSender* instance = new UndoSender(host,9090);
// 	vector<string> readIDs;
// 	readIDs.push_back("read1");
// 	readIDs.push_back("read2");

// 	vector<string> writeIDs;
// 	writeIDs.push_back("write1");
// 	writeIDs.push_back("write2");
// 	instance->newRequest("id","sourceServer",readIDs,writeIDs);
// 	instance->newRequest("id2","sourceServer2",readIDs,writeIDs);
// 	instance->sendToServer();
// }