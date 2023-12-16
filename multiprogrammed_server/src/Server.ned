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

package multiprogrammed_server;

//
// TODO auto-generated type
//
module Server
{
    parameters:
		int N = default(1);
    gates:
        // o cos� o si usa un vettore di clientIN e clientOUT
        input clientIN[N];
        output clientOUT[N];

        input webServerIN;
        output webServerOUT;

        //input hdIN;	// inutile
        //output hdOUT; // inutile

    submodules:
        hd: HardDisk;
        cpu: Cpu {
        	N = N; // o si fa lato .ini o da qui direttamente assegno parametro del server al parametro della cpu
        };
    connections:
        // dove vanno messi i delay

        cpu.hdOUT --> hd.in;
        hd.out --> cpu.hdIN;

        cpu.webServerOUT --> webServerOUT;
        webServerIN --> cpu.webServerIN;
        
        for i = 0..N-1 {
        	cpu.clientOUT[i] --> clientOUT[i];
        	clientIN[i] --> cpu.clientIN[i];
        }
}
