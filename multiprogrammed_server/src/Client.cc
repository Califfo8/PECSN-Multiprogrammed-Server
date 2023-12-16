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
    // add signals if needed

    numClients_ = par("numClients");

    /*
    cMessage * msg = new cMessage("CLI_to_CPU");
    send(msg, "out");
    */

    for( int i = 0 ; i < numClients_ ; i++ ){
        cMessage * msg = new cMessage("CLI_to_CPU");
        send(msg,"out");
    }

}

void Client::handleMessage(cMessage * msg){
    delete msg;
    cMessage * msg2 = new cMessage("CLI_to_CPU");
    send(msg2,"out"); 
}

void Client::finish(){

}

