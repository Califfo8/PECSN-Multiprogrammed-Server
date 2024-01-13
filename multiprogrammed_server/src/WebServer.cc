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

#include "WebServer.h"

Define_Module(WebServer);

void WebServer::initialize(){
    // initializing the queue
    qs_queue_  = new cQueue();
    // the server isn't busy
    working_ = false;
    // getting the values of parameters
    qs_rate_ = par("web_server_rate");
    // used for utilization
    timeWindow_ = par("timeWindow");
    // setting to zero the total time worked
    totalWorked_ = 0;
    // scheduling event related to utilization
    utilizationWsSignal_ = registerSignal("utilizationWs");
    Transaction * updateUtilizationWs = new Transaction("updateUtilizationWs");
    scheduleAt( simTime() + timeWindow_ , updateUtilizationWs );
}

void WebServer::elaborate_utilization_stat_(Transaction * msg){
    if ( working_ ){
        // if the server is working, it gets the time that 
        // isn't already counted
        totalWorked_ += ( simTime() - startWorking_ );
        startWorking_ = simTime();
    }
    //emitting the value of total time worked till that moment
    emit( utilizationWsSignal_ , totalWorked_ / simTime() );
    // scheduling a new self msg for utilization
    scheduleAt( simTime() + timeWindow_ , msg );
}

void WebServer::elaborate_msg_(Transaction * msg){
    // if not working, start working
    if(!working_)
    {
        msg->setName("Finish elaboration");
        simtime_t procTime = exponential( 1 / qs_rate_ , 0 );
        scheduleAt(simTime() + procTime, msg);
        working_ = true;
        // setting the starting time of elaboration
        startWorking_ = simTime();
    }else{
        // already working, queue insertion
        qs_queue_->insert(msg);
    }
}

void WebServer::elaborate_self_msg_(Transaction * msg){
    // if it's a self message related to utilization, elaborate the utilization
    if( strcmp(msg->getName() , "updateUtilizationWs") == 0 ){
        elaborate_utilization_stat_(msg);
        return;
    }
    // sending reply to CPU
    msg->setName("QS_to_CPU");
    send(msg,"out");
    // it stops working
    working_ = false;
    // updating total time worked
    totalWorked_ += ( simTime() - startWorking_ );
    // if queue isn't empty, gets a new msg and start elaborating it
    if(!qs_queue_->isEmpty())
    {
        msg = check_and_cast<Transaction*>( qs_queue_->pop() );
        elaborate_msg_(msg);
     }
}

void WebServer::handleMessage(cMessage * msg){
    // it starts elaborate a self msg
    if (msg->isSelfMessage() ){
        elaborate_self_msg_(check_and_cast<Transaction*>(msg));
    }else{ // else, it's an external arrival, starts to elaborate it
        elaborate_msg_(check_and_cast<Transaction*>(msg));
    }
}


void WebServer::finish(){
    // deleting all msgs in the queue
    while(!qs_queue_->isEmpty()){
        Transaction *cleaning = check_and_cast<Transaction*>(qs_queue_->pop());
        delete cleaning;
    }
    delete qs_queue_;
    // emitting last value for utilization
    emit(utilizationWsSignal_ , totalWorked_ / simTime() );
}

