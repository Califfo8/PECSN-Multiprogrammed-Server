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
        // probability of chosing a path
        double p1_;
        double p2_;
        // counter for the request that are sent back to the client
        int requestCounter_;
        // mean rate of the CPU
        double CPUmeanRate_;
        // queue for this service center
        cQueue * CPUqueue_;
        // variable that tells if the server is working or not
        bool working_;
        // variables used in the evaluation of utilization
        // time from the beginning of the last job
        simtime_t startWorking_;
        // total time in wich the server was busy
        simtime_t totalWorked_;
        // utility methods
        // elaborate the message that comes from outside
        void elaborate_external_msg_(Transaction * msg);
        // elaboratae a self message
        void elaborate_self_msg_(Transaction * msg);
        // a job is done, it sends it outside and picks a new one
        void elaborate_msg_(Transaction * msg);
        // emit the number of total transaction completed in unitOfTime_ time
        void elaborate_throughput_stat_(Transaction * msg);
        // emit the total time worked till that moment
        void elaborate_utilization_stat_(Transaction * msg);
        // statistics
        // time windows used in order to evaluate statistics
        simtime_t unitOfTime_;
        simtime_t timeWindow_;
        // signals used for evaluating statistics
        simsignal_t requestCounterSignal_;
        simsignal_t utilizationCpuSignal_;
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

#endif /* CPU_H_ */
