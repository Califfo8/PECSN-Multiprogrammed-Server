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

#include "Client.h"

Define_Module(Client);

void Client::initialize(){
    numClients_ = par("numClients");
    responseTimeSignal_ = registerSignal("responseTime");
    for( int i = 0 ; i < numClients_ ; i++ ){
        Transaction * msg = new Transaction("CLI_to_CPU");
        msg->setID(i);
        msg->setStartTransaction(simTime());
        send(msg,"out");
    }
}

void Client::handleMessage(cMessage * msg){
    Transaction * tempMsg = check_and_cast<Transaction*>(msg);
    simtime_t arrivalTime = simTime();
    simtime_t departureTime = tempMsg->getStartTransaction();
    emit(responseTimeSignal_, arrivalTime - departureTime );
    delete tempMsg;
    Transaction * msg2 = new Transaction("CLI_to_CPU");
    msg2->setStartTransaction(simTime());
    send(msg2,"out"); 
}

void Client::finish(){

}

