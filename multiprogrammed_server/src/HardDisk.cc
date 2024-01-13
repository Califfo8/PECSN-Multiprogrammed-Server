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

#include "HardDisk.h"

Define_Module(HardDisk);

void HardDisk::initialize(){
    // getting the values of parameters
    rate_ = par("rateProcessing");
    timeWindow_ = par("timeWindow");
    // the server is not working
    working_ = false;
    // setting to zero the total time worked
    totalWorked_ = 0;
    // signal for utilization
    utilizationHdSignal_ = registerSignal("utilizationHd");
    // initializing the queue
    hd_queue_ = new cQueue();
    // message for the utilization sampling
    Transaction *msg = new Transaction("updateUtilizationHd");
    scheduleAt( simTime() + timeWindow_ , msg);
}
void HardDisk::elaborate_msg_(Transaction * msg)
{
    // if the server is free, it starts to work 
    if(!working_)
    {
        msg->setName("Finish elaboration");
        simtime_t procTime = exponential( 1 / rate_ , 0 );
        scheduleAt(simTime() + procTime, msg);
        working_ = true;
        // setting the initial time of elaboration
        startWorking_ = simTime();
    }else{
        // the server is busy, inserting the msg in the queue 
        hd_queue_->insert(msg);
    }
}

void HardDisk::elaborate_self_msg_(Transaction * msg)
{   
    // update utilization
    if( strcmp(msg->getName() , "updateUtilizationHd") == 0 ){
        if ( working_ ){
            // if the server is working, it gets the time that 
            // isn't already counted
            totalWorked_ += ( simTime() - startWorking_ );
            startWorking_ = simTime();
        }
        // emitting the value of total time worked till that moment
        emit(utilizationHdSignal_ , totalWorked_ / simTime() );
        // scheduling a new self msg for the statistic
        scheduleAt( simTime() + timeWindow_ , msg);
        return;
    }
    // An elaboration is done
    // send a reply msg
    msg->setName("HD_to_CPU");
    send(msg, "out");
    working_ = false;
    // updating total time in wich the server was busy    
    totalWorked_ += ( simTime() - startWorking_ );
    // if the queue isn't empty, it gets a new msg from the queue and elaborates it
    if(!hd_queue_->isEmpty())
    {
        msg = check_and_cast<Transaction*>( hd_queue_->pop() );
        elaborate_msg_(msg);
    }
}

void HardDisk::handleMessage(cMessage * msg){
    // msg is a self message
    if(msg->isSelfMessage())
        elaborate_self_msg_(check_and_cast<Transaction*>(msg));
    else // it's from the CPU
        elaborate_msg_(check_and_cast<Transaction*>(msg));
}

void HardDisk::finish(){
    // deleting all the msgs in the queue
    while(!hd_queue_->isEmpty()){
        cMessage *cleaning = check_and_cast<cMessage*>(hd_queue_->pop());
        delete cleaning;
    }
    delete hd_queue_;
    // emitting last value for utilization
    emit(utilizationHdSignal_ , totalWorked_ / simTime() );
}
