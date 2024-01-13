//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <omnetpp.h>
#include "transaction_m.h"
using namespace omnetpp;

class WebServer : public cSimpleModule {
    // NOTE: "_" in name stands of properties stands for private, as seen during lectures
    private:
        // mean rate of WS
        double qs_rate_;
        // queue for this server center
        cQueue * qs_queue_;
        // it states if the server is working or not
        bool working_;

        // used for evaluation of utilization
        simtime_t startWorking_;
        simtime_t totalWorked_;
        simtime_t timeWindow_;
        // utility methods
        void elaborate_self_msg_(Transaction * msg);
        void elaborate_utilization_stat_(Transaction * msg);
        void elaborate_msg_(Transaction * msg);
        // utilization
        simsignal_t utilizationWsSignal_;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

#endif /* WEBSERVER_H_ */
