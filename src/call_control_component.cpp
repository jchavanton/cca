#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

using namespace std;

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/client_simple.hpp>

int kam_core_version(string const kamXmlRpc) {
	try {
		string const methodName("core.version");
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		myClient.call(kamXmlRpc, methodName, &result);
		string kamailioVersion = xmlrpc_c::value_string(result);
		cout<<kamXmlRpc<<" running version:"<<kamailioVersion<<endl;
	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
}

map<string, xmlrpc_c::value> kam_dlg_dlg_list(string const kamXmlRpc, string const &callId) {
	try {
		string const methodName("dlg.dlg_list");
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		xmlrpc_c::paramList paramList;
		xmlrpc_c::value_string callIdParam(callId);
		paramList.add(callIdParam);

		myClient.call(kamXmlRpc, methodName, paramList, &result);

		if (result.type() != xmlrpc_c::value::TYPE_STRUCT) {
				map<string, xmlrpc_c::value> empty;
				return empty;
		}
		xmlrpc_c::value_struct call_struct(result);
		map<string, xmlrpc_c::value> callInfo(call_struct);
		string callId = xmlrpc_c::value_string(callInfo["call-id"]);
		// callee info
		xmlrpc_c::value_struct callee_struct(callInfo["callee"]);
		map<string, xmlrpc_c::value> calleeInfo(callee_struct);
		string totag = xmlrpc_c::value_string(calleeInfo["tag"]);
		// caller info
		xmlrpc_c::value_struct caller_struct = xmlrpc_c::value_struct(callInfo["caller"]);
		map<string, xmlrpc_c::value> caller_cstruct(caller_struct);
		string fromtag = xmlrpc_c::value_string(caller_cstruct["tag"]);
		cout << "call["<< callId <<"]fromtag["<< fromtag <<"]totag["<< totag <<"]"<< endl;
		return callInfo;

	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
}

int kam_dlg_list(string const kamXmlRpc) {
	try {
		string const methodName("dlg.list");
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		myClient.call(kamXmlRpc, methodName, &result);
		xmlrpc_c::value_array arrayData(result);
		cout << "calls : " << arrayData.size() << endl;
		vector<xmlrpc_c::value> callValues = arrayData.vectorValueValue();
		while (!callValues.empty()) {
			xmlrpc_c::value call = callValues.back();
			if (call.type() == xmlrpc_c::value::TYPE_STRUCT) {
				xmlrpc_c::value_struct call_struct(call);
				map<string, xmlrpc_c::value> callInfo(call_struct);
				string callId = xmlrpc_c::value_string(callInfo["call-id"]);
				// cout << "call["<< callId <<"]" << endl;
				kam_dlg_dlg_list(kamXmlRpc, callId);
			}
			callValues.pop_back();
		}
	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
}

int kam_dlg_terminate_dlg(string const kamXmlRpc, string const callId, string const fromTag, string const toTag) {
	try {
		string const methodName("dlg.terminate_dlg");
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		xmlrpc_c::paramList paramList;
		xmlrpc_c::value_string callIdParam(callId);
		paramList.add(callIdParam);
		xmlrpc_c::value_string fromTagParam(fromTag);
		paramList.add(fromTagParam);
		xmlrpc_c::value_string toTagParam(toTag);
		paramList.add(toTagParam);
		myClient.call(kamXmlRpc, methodName, paramList, &result);

	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
}

class MethodDlgTerminate : public xmlrpc_c::method {
public:
	MethodDlgTerminate(string const kamXmlRpc) {
		this->_kamXmlRpc = kamXmlRpc;
		this->_signature = "i:ss";
		// method's result is int and 3 arguments are strings
		this->_help = "This method terminate a call";
	}
	void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value * const retvalP) {
		string const method(paramList.getString(0));
		string const callId(paramList.getString(1));
		// string const fromTag(paramList.getString(2));
		paramList.verifyEnd(3);

		map<string, xmlrpc_c::value> callInfo = kam_dlg_dlg_list(_kamXmlRpc, callId);
		if (callInfo.empty()) {
				*retvalP = xmlrpc_c::value_int(404);
				return;
		}
		*retvalP = xmlrpc_c::value_int(200);
		// caller info
		xmlrpc_c::value_struct caller_struct(callInfo["caller"]);
		map<string, xmlrpc_c::value> callerInfo(caller_struct);
		string fromTag = xmlrpc_c::value_string(callerInfo["tag"]);
		// callee info
		xmlrpc_c::value_struct callee_struct(callInfo["callee"]);
		map<string, xmlrpc_c::value> calleeInfo(callee_struct);
		string toTag = xmlrpc_c::value_string(calleeInfo["tag"]);
		// Disconnect the call on Kamailio
		kam_dlg_terminate_dlg(_kamXmlRpc, callId, fromTag, toTag);
	}
private:
	string _kamXmlRpc;
};

int main(int const argc, const char *argv[]) {
	cout<<"\nusage: "<< argv[0] << " <local_port> <kamailio_ip:port> <log_filename>\n\n";
	int port = 8080;
	if (argc > 1) port = atoi(argv[1]);
	string abyssLogFileName = "/tmp/abyss.log";
	if (argc > 2) abyssLogFileName.assign(argv[2]);
	string kamXmlRpc = "http://";
	if (argc > 3) kamXmlRpc = kamXmlRpc + argv[3] + "/RPC2";
	else kamXmlRpc = kamXmlRpc + "127.0.0.1:4291/RPC2";

	kam_core_version(kamXmlRpc);
	kam_dlg_list(kamXmlRpc);
	try {
		xmlrpc_c::registry myRegistry;
		xmlrpc_c::methodPtr const MethodDlgTerminateP(new MethodDlgTerminate(kamXmlRpc));
		myRegistry.addMethod("mi", MethodDlgTerminateP);

		xmlrpc_c::serverAbyss myAbyssServer(
			xmlrpc_c::serverAbyss::constrOpt()
				.registryP(&myRegistry)
				.portNumber(port)
				.logFileName(abyssLogFileName)
		);
		cout << "server listening on port :"<< port << endl;
		myAbyssServer.run();
		// xmlrpc_c::serverAbyss.run() never returns
		assert(false);
	} catch (exception const& e) {
		cerr << "Something failed.  " << e.what() << endl;
	}
	return 0;
}
