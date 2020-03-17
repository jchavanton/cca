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
		cerr << "[core.version] client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "[core.version] client threw unexpected error." << endl;
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
				cout << "[dlg.dlg_list] call not found\n";
				map<string, xmlrpc_c::value> empty;
				return empty;
		}
		xmlrpc_c::value_struct call_struct(result);
		map<string, xmlrpc_c::value> callInfo(call_struct);
		string callId = xmlrpc_c::value_string(callInfo["call-id"]);
		int callState = xmlrpc_c::value_int(callInfo["state"]);
		// callee info
		xmlrpc_c::value_struct callee_struct(callInfo["callee"]);
		map<string, xmlrpc_c::value> calleeInfo(callee_struct);
		string totag = xmlrpc_c::value_string(calleeInfo["tag"]);
		// caller info
		xmlrpc_c::value_struct caller_struct = xmlrpc_c::value_struct(callInfo["caller"]);
		map<string, xmlrpc_c::value> caller_cstruct(caller_struct);
		string fromtag = xmlrpc_c::value_string(caller_cstruct["tag"]);
		string output = "[dlg.dlg_list] call["+callId+"]fromtag["+fromtag+"]totag["+totag+"]state["+to_string(callState)+"]\n";
		cout << output;
		return callInfo;

	} catch (exception const& e) {
		cerr << "[dlg.dlg_list] client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "[dlg.dlg_list] client threw unexpected error." << endl;
	}
	map<string, xmlrpc_c::value> callInfo;
	return callInfo;
}

std::tuple<int, string> kam_dlg_list(string const kamXmlRpc) {
	try {
		string const methodName("dlg.list");
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		myClient.call(kamXmlRpc, methodName, &result);
		xmlrpc_c::value_array arrayData(result);
		string output = "calls : " + to_string( arrayData.size()) + "\n";
		cout << output;
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
		cerr << "[dlg.list] client threw error: " << e.what() << endl;
		return make_tuple(500, e.what());
	} catch (...) {
		cerr << "[dlg.list] client threw unexpected error." << endl;
		return make_tuple(500, "unexpected error");
	}
	return make_tuple(200, "OK");
}

std::tuple<int, string> kam_dlg_terminate_dlg(string const kamXmlRpc, string const callId, string const fromTag, string const toTag) {
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
		cerr << "[dlg.terminate_dlg] client threw error: " << e.what() << endl;
		return make_tuple(500, e.what());
	} catch (...) {
		cerr << "[dlg.terminate_dlg] client threw unexpected error." << endl;
		return make_tuple(500, "unexpected error");
	}
	cout << "[dlg.terminate_dlg] 200 OK" << endl;
	return make_tuple(200, "OK");
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
		string const fromTag(paramList.getString(2));

		string output = "[received][mi] method["+method+"]callid["+callId+"]fromtag["+fromTag+"]\n";
		cout << output;
		paramList.verifyEnd(3);
		map<string, xmlrpc_c::value> result;

		map<string, xmlrpc_c::value> callInfo = kam_dlg_dlg_list(_kamXmlRpc, callId);
		if (callInfo.empty()) {
				result["faultCode"] = xmlrpc_c::value_int(404);
				result["faultString"] = xmlrpc_c::value_string("Dialog not found");
				*retvalP = xmlrpc_c::value_struct(result);
				return;
		}

		// // caller info
		// xmlrpc_c::value_struct caller_struct(callInfo["caller"]);
		// map<string, xmlrpc_c::value> callerInfo(caller_struct);
		// string fromTag = xmlrpc_c::value_string(callerInfo["tag"]);
		// callee info
		xmlrpc_c::value_struct callee_struct(callInfo["callee"]);
		map<string, xmlrpc_c::value> calleeInfo(callee_struct);
		string toTag = xmlrpc_c::value_string(calleeInfo["tag"]);
		// Disconnect the call on Kamailio
		tuple<int, string> res = kam_dlg_terminate_dlg(_kamXmlRpc, callId, fromTag, toTag);
		result["faultCode"] = xmlrpc_c::value_int(get<0>(res));
		result["faultString"] = xmlrpc_c::value_string(get<1>(res));
		*retvalP = xmlrpc_c::value_struct(result);
	}
private:
	string _kamXmlRpc;
};

int main(int argc, char **argv) {
	cout<<"\nusage: "<< argv[0] << " -p <local_port> -s <kamailio_ip:port> -l <log_filename> -m <max_connections> -c show active calls at startup \n\n";

	int port = 8080;
	string kamXmlRpc = "http://127.0.0.1:4291/RPC2";
	string abyssLogFileName;
	int maxConn{250};
	int maxConnBacklog{250};
	int opt;
	while ((opt = getopt(argc, argv, "p:s:l:c:m:")) != -1) {
		switch (opt) {
			case 'p':
				port = atoi(optarg);
				break;
			case 's':
				kamXmlRpc = "";
				kamXmlRpc = kamXmlRpc+"http://"+optarg+"/RPC2";
				break;
			case 'l':
				abyssLogFileName = optarg;
				break;
			case 'm':
				maxConn = atoi(optarg);
				maxConnBacklog = maxConn;
				break;
			case 'c':
				kam_dlg_list(kamXmlRpc);
				break;
		}
	}

	kam_core_version(kamXmlRpc);

	try {
		xmlrpc_c::registry myRegistry;
		xmlrpc_c::methodPtr const MethodDlgTerminateP(new MethodDlgTerminate(kamXmlRpc));
		myRegistry.addMethod("mi", MethodDlgTerminateP);

		xmlrpc_c::serverAbyss::constrOpt abyssOpt;
		abyssOpt.registryP(&myRegistry);
		abyssOpt.portNumber(port);
		abyssOpt.maxConn(maxConn);
		abyssOpt.maxConnBacklog(maxConnBacklog);
		if (abyssLogFileName.length() > 0)
				abyssOpt.logFileName(abyssLogFileName);
		xmlrpc_c::serverAbyss myAbyssServer(abyssOpt);
		cout << "server listening on port: "<< port << " max connections:"<< maxConn <<endl;
		myAbyssServer.run();
		// xmlrpc_c::serverAbyss.run() never returns
		assert(false);
	} catch (exception const& e) {
		cerr << "Something failed.  " << e.what() << endl;
	}
	return 0;
}
