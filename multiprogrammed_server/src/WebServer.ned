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
simple WebServer
{
    parameters:
        // mean rate of Web Server
        double web_server_rate = default(1);
        // time window used for utilization        
        int timeWindow @unit(s) = default(10s);
        // utilization
        @signal[utilizationWs](type=double);
        @statistic[utilizationWsStat](source="utilizationWs";record=mean,vector);
   	gates:
   	    input in;
   	    output out;
}
