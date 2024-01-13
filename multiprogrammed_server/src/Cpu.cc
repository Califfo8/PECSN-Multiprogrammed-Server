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

#include "Cpu.h"

Define_Module(Cpu);

void Cpu::initialize(){
    // getting the values of parameters
    p1_ = par("p1");
    p2_ = par("p2");
    CPUmeanRate_ = par("rate");

    // initializing the total number of transaction done
    requestCounter_ = 0;
    // initializing the queue
    CPUqueue_ = new cQueue();
    // the server isn't busy 
    working_ = false;

    // getting the values for parameters related to signals
    unitOfTime_ = par("unitOfTime");
    requestCounterSignal_ = registerSignal("requestCounter");
    // setting to zero the total time worked
    totalWorked_ = 0;
    // getting the values for parameters related to signals
    timeWindow_ = par("timeWindow");
    utilizationCpuSignal_ = registerSignal("utilizationCpu");

    // scheduling events related to signals
    Transaction * updateThroughput = new Transaction("updateThroughput");
    scheduleAt( simTime() + unitOfTime_ , updateThroughput );

    Transaction * updateUtilizationCpu = new Transaction("updateUtilizationCpu");
    scheduleAt( simTime() + timeWindow_ , updateUtilizationCpu );
}

void Cpu::elaborate_msg_(Transaction * msg)
{
    // getting the probability for chosing a path
    double prob = uniform(0,1,1);
    // deciding the path accordingly
    // setting the name accordingly
    // sending the Transaction out
    // if is destined to a client the number of requested served is incremented
    if ( prob <= p1_ ){
         msg->setName("END_T");
         requestCounter_++;
         send(msg,"clientOUT");
    }else if ( prob > p1_ && prob <= p1_ + p2_ ){
         msg->setName("CPU_to_HD");
         send(msg,"hdOUT");
    }else{
         msg->setName("CPU_to_QS");
         send(msg,"webServerOUT");
    }
    // job served
    working_ = false;
    // gathering data related to utilization
    totalWorked_ += ( simTime() - startWorking_ );

    // if there is a job in the queue, CPU starts serving it
    if ( !CPUqueue_->isEmpty() ){
        // setting values related to utilization
        startWorking_ = simTime();
        // the server starts working
        working_ = true;
        // getting the first job in the queue
        Transaction *msg2 = check_and_cast<Transaction*>( CPUqueue_->pop() );
        msg->setName("Job_served");
        simtime_t procTime = exponential( 1 / CPUmeanRate_ , 0);
        // scheduling the end of the work
        scheduleAt( simTime() + procTime , msg2 );
    }
}

void Cpu::elaborate_throughput_stat_(Transaction * msg){
    // emit the value of total number of request served
    emit(requestCounterSignal_ , requestCounter_ );
    // resets the number
    requestCounter_ = 0;
    // scheduling a new self msg for the statistic
    scheduleAt( simTime() + unitOfTime_ , msg );
}

void Cpu::elaborate_utilization_stat_(Transaction * msg){
    if ( working_ ){
        // if the server is working, it gets the time that 
        // isn't already counted
        totalWorked_ += ( simTime() - startWorking_ );
        startWorking_ = simTime();
    }
    // emitting the value of total time worked till that moment
    emit( utilizationCpuSignal_ , totalWorked_ / simTime() );
    // scheduling a new self msg for the statistic
    scheduleAt( simTime() + timeWindow_ , msg );
}

void Cpu::elaborate_self_msg_(Transaction * msg){
    // if it's a self message the right type of message is selected
    if( strcmp(msg->getName() , "updateThroughput" ) == 0 ){
        elaborate_throughput_stat_(msg);
    }else if( strcmp(msg->getName() , "updateUtilizationCpu") == 0 ){
        elaborate_utilization_stat_(msg);
    }else{
        elaborate_msg_(msg);
    }
}

void Cpu::elaborate_external_msg_(Transaction * msg){
    // CPU gets a msg from another module
    // if it's working the new job is inserted in the queue
    if ( working_ ){
            CPUqueue_->insert(msg);
    }else{
        // CPU is free and starts to elaborate the new msg
        // utilization statistic
        startWorking_ = simTime();
        // the server starts to work
        working_ = true;

        // scheduling the end of elaboration msg
        msg->setName("Job_served");
        simtime_t procTime = exponential( 1 / CPUmeanRate_ , 0 );
        scheduleAt( simTime() + procTime , msg );
    }
}


void Cpu::handleMessage(cMessage * msg){
    // cast the msg to Transaction and decide if the msg is self or not
    Transaction * tempMsg = check_and_cast<Transaction*>(msg);
    if ( tempMsg->isSelfMessage() ){
        elaborate_self_msg_(tempMsg);
    }else{
        elaborate_external_msg_(tempMsg);
    }
}

void Cpu::finish(){
    // deleting all the msgs in the queue
    while ( !CPUqueue_->isEmpty() ){
        Transaction *msg = check_and_cast<Transaction*>( CPUqueue_->pop() );
        delete msg;
    }
    delete CPUqueue_;
    // emitting last value for utilization
    emit(utilizationCpuSignal_ , totalWorked_ / simTime() );
}

