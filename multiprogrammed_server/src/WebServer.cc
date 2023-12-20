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

}

void WebServer::handleMessage(cMessage * msg){
    if(!msg->isSelfMessage()){
        if(!working_){
            scheduleAt(simTime() + exponential( 1 / qs_rate_ , 0 ) , msg );
            working_ = true;
        }
        else {
            qs_queue_->insert(msg);
        }
    }
    else {
        msg->setName("QS_to_CPU");
        send(msg,"out");
        if(qs_queue_->isEmpty()){
            working_ = false;
        }
        else {
            cMessage *self = check_and_cast <cMessage* > (qs_queue_->pop());
            scheduleAt(simTime()+ exponential(1/qs_rate_), self);
            working_ = true;
        }
    }
}

void WebServer::finish(){
    while(!qs_queue_->isEmpty()){
        cMessage *cleaning = check_and_cast<cMessage*>(qs_queue_->pop());
        delete cleaning;
    }
    delete qs_queue_;
}

