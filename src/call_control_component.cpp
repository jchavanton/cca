// #define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/server_abyss.hpp */



#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

using namespace std;

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

class MethodDlgTerminate : public xmlrpc_c::method {
public:
	MethodDlgTerminate() {
		this->_signature = "i:sss";
		// method's result is int and 3 arguments are strings
		this->_help = "This method terminate a call";
	}
	void execute(xmlrpc_c::paramList const& paramList, xmlrpc_c::value * const retvalP) {
		string const callid(paramList.getString(0));
		string const fromtag(paramList.getString(1));
		string const totag(paramList.getString(2));
		paramList.verifyEnd(3);
		*retvalP = xmlrpc_c::value_int(1);
		// Disconnect the call on Kamailio
	}
};

int main(int const, const char ** const) {
	try {
		xmlrpc_c::registry myRegistry;
		xmlrpc_c::methodPtr const MethodDlgTerminateP(new MethodDlgTerminate);
		myRegistry.addMethod("dlg.terminate_dlg", MethodDlgTerminateP);

		xmlrpc_c::serverAbyss myAbyssServer(
			xmlrpc_c::serverAbyss::constrOpt()
				.registryP(&myRegistry)
				.portNumber(8080)
				.logFileName("/tmp/abyss.log")
		);
		cout << "server starting 8080" << endl;
		myAbyssServer.run();
		// xmlrpc_c::serverAbyss.run() never returns
		assert(false);
	} catch (exception const& e) {
		cerr << "Something failed.  " << e.what() << endl;
	}
	return 0;
}
