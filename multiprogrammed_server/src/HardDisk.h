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

#ifndef HARDDISK_H_
#define HARDDISK_H_

#include <omnetpp.h>
#include "transaction_m.h"

using namespace omnetpp;

class HardDisk : public cSimpleModule {
    // NOTE: "_" in name stands of properties stands for private, as seen during lectures
    private:
        // mean rate of HDD
        double rate_;
        // tells if the server is working or not
        bool working_;
        // queue for the server center
        cQueue* hd_queue_;
        // variables used for evaluating utilization
        simtime_t startWorking_;
        simtime_t totalWorked_;
        simtime_t timeWindow_;
        // utility methods
        void elaborate_msg_(Transaction * msg);
        void elaborate_self_msg_(Transaction * msg);

        // utilization
        simsignal_t utilizationHdSignal_;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

#endif /* HARDDISK_H_ */
