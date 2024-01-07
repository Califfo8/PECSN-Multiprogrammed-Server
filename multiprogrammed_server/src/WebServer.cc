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

    qs_queue_  = new cQueue();
    working_ = false;
    qs_rate_ = par("web_server_rate");

    timeWindow_ = par("timeWindow");

    utilizationWsSignal_ = registerSignal("utilizationWs");
    Transaction * updateUtilizationWs = new Transaction("updateUtilizationCpu");
    scheduleAt( simTime() + timeWindow_ , updateUtilizationWs );
}

void WebServer::elaborate_utilization_stat_(Transaction * msg){
    emit( utilizationWsSignal_ , totalWorked_ / simTime() );
    scheduleAt( simTime() + timeWindow_ , msg );
}

void WebServer::elaborate_msg_(Transaction * msg){
    msg->setName("QS_to_CPU");
    send(msg,"out");
    if(qs_queue_->isEmpty()){
        working_ = false;
        totalWorked_ += ( simTime() - startWorking_ );
    }
    else {
        Transaction *self = check_and_cast <Transaction*> (qs_queue_->pop());
        scheduleAt(simTime()+ exponential( 1 / qs_rate_ , 0 ), self);
        working_ = true; // useless?
    }
}

void WebServer::elaborate_self_msg_(Transaction * msg){
    if( strcmp(msg->getName() , "updateUtilizationWs") == 0 ){
        elaborate_utilization_stat_(msg);
    }else{
        elaborate_msg_(msg);
    }
}

void WebServer::elaborate_external_msg_(Transaction * msg){
    if( !working_ ){
        startWorking_ = simTime();
        working_ = true;
        // msg->setName("Job_served");
        // simtime_t procTime --> sostituire sotto se vogliamo
        scheduleAt(simTime() + exponential( 1 / qs_rate_ , 0 ) , msg );
    }else{
        qs_queue_->insert(msg);
    }
}

void WebServer::handleMessage(cMessage * msg){
   if (msg->isSelfMessage() ){
        elaborate_self_msg_(check_and_cast<Transaction*>(msg));
    }else{
        elaborate_external_msg_(check_and_cast<Transaction*>(msg));
    }
}


void WebServer::finish(){
    while(!qs_queue_->isEmpty()){
        Transaction *cleaning = check_and_cast<Transaction*>(qs_queue_->pop());
        delete cleaning;
    }
    delete qs_queue_;

    emit(utilizationWsSignal_ , totalWorked_ / simTime() );
}

