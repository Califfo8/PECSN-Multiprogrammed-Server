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
    hd_queue_ = new cQueue();
}
void HardDisk::elaborate_msg_(Transaction * msg)
{
    //Se non sto elaborando alcun messaggio, quindi lo elaboro
    if(!working_)
    {
        msg->setName("Finish elaboration");
        simtime_t procTime = exponential( 1 / rate_ , 0 );
        scheduleAt(simTime() + procTime, msg);
        working_ = true;
    }else{
     //Altrimenti ho già un elaborazione in corso, e il messaggio si accoda
        hd_queue_->insert(msg);
    }

}

void HardDisk::elaborate_self_msg_(Transaction * msg)
{
    //Invio il messaggio di risposta
            msg->setName("HD_to_CPU");
            send(msg, "out");
            working_ = false;

            //Se la coda è piena prendo un altro messaggio e riparto
            if(!hd_queue_->isEmpty())
            {
                msg = check_and_cast<Transaction*>( hd_queue_->pop() );
                elaborate_msg_(msg);
            }
}

void HardDisk::handleMessage(Transaction * msg){

    // Il messaggio è di fine elaborazione
    if(msg->isSelfMessage())
        elaborate_self_msg_(msg);
    else//Il messaggio è della CPU
        elaborate_msg_(msg);


}

void HardDisk::finish(){
        while(!hd_queue_->isEmpty()){
            cMessage *cleaning = check_and_cast<cMessage*>(hd_queue_->pop());
            delete cleaning;
        }
        delete hd_queue_;
}
