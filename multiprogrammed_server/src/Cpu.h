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

#ifndef CPU_H_
#define CPU_H_

#include <omnetpp.h>
#include "transaction_m.h"

using namespace omnetpp;

class Cpu : public cSimpleModule {

    // NOTE: "_" in name stands of properties stands for private, as seen during lectures
    private:
        double p1_;
        double p2_;
        int requestCounter_;
        double CPUmeanRate_;
        cQueue * CPUqueue_;
        bool working_;
        simtime_t startWorking_;
        simtime_t totalWorked_;
        void elaborate_self_msg_(Transaction * msg);
        void elaborate_external_msg_(Transaction * msg);
        void elaborate_msg_(Transaction * msg);
        void elaborate_throughput_stat_(Transaction * msg);
        // statistic
        simtime_t unitOfTime_;
        simsignal_t requestCounterSignal_;
        simsignal_t utilizationCpuSignal_;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

#endif /* CPU_H_ */
