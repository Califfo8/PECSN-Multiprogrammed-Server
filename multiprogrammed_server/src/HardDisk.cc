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
    rate_ = par("rateProcessing");
    working_ = false;
    // add signals if needed

    // add code for managing a queue
    cQueue * hd_queue_ = new cQueue();
}
void HardDisk::elaborate_msg_(cMessage * msg)
{
    //Se non sto elaborando alcun messaggio, quindi lo elaboro
    if(!working_)
    {
        finish_comp_ = new cMessage("Finish_Computation");
        scheduleAt(simTime() + exponential(1/rate_), finish_comp_);
        working_ = true;
        workOnMsg = msg;
    }else{
     //Altrimenti ho già un elaborazione in corso, e il messaggio si accoda
        hd_queue_->insert(msg);
    }

}
void HardDisk::handleMessage(cMessage * msg){
    //Il messaggio è della CPU
    if( strcmp(msg->getName(),"CPU_to_HD") == 0)
    {
        elaborate_msg_(msg);
    } // Il messaggio è di fine elaborazione
    else if(msg->isSelfMessage())
    {
    //Invio il messaggio di risposta
        workOnMsg->setName("HD_to_CPU");
        send(workOnMsg, "out");
        working_ = false;
        //Se la coda non è vuota prendo un altro messaggio e riparto
        if(!hd_queue_->isEmpty())
        {
            msg = check_and_cast<cMessage*>( hd_queue_->pop() );
            elaborate_msg_(msg);
        }
    }


}

void HardDisk::finish(){

}