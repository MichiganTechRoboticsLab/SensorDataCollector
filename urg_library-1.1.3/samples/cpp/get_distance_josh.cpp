
#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include "IRLServer.h"
#include <iostream>

#include<iostream>
#include<string>
#include<unistd.h>

#include"IRLServer.h"

using namespace qrk;
using namespace std;


int main(int argc, char *argv[])
{
    Connection_information information(argc, argv);
//josh
    IRLServer serv(1140);
    unsigned char stuff = 5;
    
    Urg_driver urg;
    if (!urg.open(information.device_or_ip_name(),
                  information.baudrate_or_port_number(),
                  information.connection_type())) {
        cout << "Urg_driver::open(): "
             << information.device_or_ip_name() << ": " << urg.what() << endl;
        return 1;
    }

    urg.start_measurement(Urg_driver::Distance, 0, 0);
    for (int i = 0; i > -1; ++i) {
        vector<long> data;
        long time_stamp = 0;
        long ident = 0xDEADBEEF;
        
        if (!urg.get_distance(data, &time_stamp)) {
            cout << "Urg_driver::get_distance(): " << urg.what() << endl;
            return 1;
        }

		data.push_back(time_stamp);
		data.push_back(ident);
		
		int temp = data.size();
		
		std::cout << "ip address: " << argv[1] << std::endl;
        serv.send(&data[0],sizeof(data[0])*data.size(), 3540, argv[1]);
        
    }

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
