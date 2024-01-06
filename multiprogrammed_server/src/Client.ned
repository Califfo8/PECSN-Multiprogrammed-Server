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
simple Client
{
    parameters:
    	int numClients = default(1);
    	// quindi si fa che questo simple Module rappresenta N client
    	// o si usano N client separati ( da come parlato sabato sera la prima soluzione)
    	// nel caso i messaggi devono comprendere che client �?
		@signal[responseTime](type=double);
        @statistic[responseTimeStat](source="responseTime";record=mean);
    gates: 
        input in;
        output out;
}
