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
    p1_ = par("p1");
    p2_ = par("p2");
    CPUmeanRate_ = par("rate");
    requestCounter_ = 0;
    // add signals
    queue_ = new cQueue();
    working_ = false;
    // add code for managing a queue

    unitOfTime_ = par("unitOfTime");
    requestCounterSignal_ = registerSignal("requestCounter");

    cMessage * updateThroughput = new cMessage("updateThroughput");
    scheduleAt( simTime() + unitOfTime_ , updateThroughput );
}

void Cpu::elaborate_msg_(cMessage * msg)
{
   double prob = uniform(0,1,1);
   
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

   if ( queue_->isEmpty() ){
       working_ = false;
   }else{
       cMessage *msg2 = check_and_cast<cMessage*>( queue_->pop() );
       simtime_t procTime = exponential( 1 / CPUmeanRate_ , 0);
       scheduleAt( simTime() + procTime , msg2 );
   }
}

void Cpu::elaborate_throughput_stat_(cMessage * msg){
    emit(requestCounterSignal_ , requestCounter_ );
    requestCounter_ = 0;
    scheduleAt( simTime() + unitOfTime_ , msg );
}

void Cpu::elaborate_self_msg_(cMessage * msg){
    if( strcmp(msg->getName() , "updateThroughput" ) == 0 ){
        elaborate_throughput_stat_(msg);
    }else{
        elaborate_msg_(msg);
    }
}

void Cpu::elaborate_external_msg_(cMessage * msg){
    if ( working_ ){
            queue_->insert(msg);
    }else{
        working_ = true;
        //strcpy(msg,msg_); // mi salvo il messaggio che mi hanno mandato non so se è utile
        //strcpy(msg->getName(), msg_); 
        //msg_ = msg->dup();
        cMessage * msg2 = new cMessage("job_served");
        simtime_t procTime = exponential( 1 / CPUmeanRate_ );
        scheduleAt( simTime() + procTime , msg2 );
    }
}

void Cpu::handleMessage(cMessage * msg){
    // arrivo messaggio
        // self_message 
            // elaborazione finita
                // calcolo la probabilità 
                    // mando in uscita nel punto corretto
                        // prob <= p1 
                            // messaggio al client 
                        // p1 < prob <= p2
                            // messaggio all'hd
                        // p1 + p2 < prob
                            // messaggio al webServer
                // se coda è libera smetto di lavorare
                // altrimenti prendo il prossimo dalla coda
            // sono passate le unitOfTime
                // bisogna calcolare il throughput
        // messaggio da esterno
            // server lavora
                // accodo il messaggio
            // server libero
                // lo mando subito a lavorare (schedule at)

    if ( msg->isSelfMessage() ){
        elaborate_self_msg_(msg);
    }else{
        elaborate_external_msg_(msg);
    }
}

void Cpu::finish(){

}

