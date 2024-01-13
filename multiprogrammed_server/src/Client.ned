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
        // this variabile indicates the number of clients that interact 
        // with the system
    	int numClients = default(1);
        // signal used for extracting the mean response time of the system
		@signal[responseTime](type=double);
        @statistic[responseTimeStat](source="responseTime";record=mean,vector);
    gates: 
        input in;
        output out;
}
