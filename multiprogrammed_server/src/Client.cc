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
    // initializing the number of clients of the system
    numClients_ = par("numClients");
    // registering the signal
    responseTimeSignal_ = registerSignal("responseTime");
    // sending numClients_ Transaction to the cpu(inside the server)
    for( int i = 0 ; i < numClients_ ; i++ ){
        Transaction * msg = new Transaction("CLI_to_CPU");
        // setting the ID of the transaction
        msg->setID(i);
        // setting the time in wich the transaction starts
        // useful for evaluating the response time of transaction
        msg->setStartTransaction(simTime());
        // sending the Transaction
        send(msg,"out");
    }
}

void Client::handleMessage(cMessage * msg){
    // casting to Transaction from cMessage
    Transaction * tempMsg = check_and_cast<Transaction*>(msg);
    // getting the time values of arrival and departure
    simtime_t arrivalTime = simTime();
    simtime_t departureTime = tempMsg->getStartTransaction();
    // emitting the difference (response time of this Transaction)
    emit(responseTimeSignal_, arrivalTime - departureTime );
    // deleting the msg
    delete tempMsg;
    // creating a new one
    Transaction * msg2 = new Transaction("CLI_to_CPU");
    msg2->setStartTransaction(simTime());
    send(msg2,"out"); 
}

void Client::finish(){

}

