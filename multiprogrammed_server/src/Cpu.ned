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
simple Cpu
{
    parameters:
        double p1 = default(0.3); 
        double p2 = default(0.3);
        double rate = default(1);
        
		// aggiungere le statistic
        int unitOfTime @unit(s) = default(1s);
        int timeWindow @unit(s) = default(10s);
        // throughput
        @signal[requestCounter](type=long);
        @statistic[requestCounterStat](source="requestCounter";record=mean,vector);

        // utilization
        @signal[utilizationCpu](type=double);
        @statistic[utilizationCpuStat](source="utilizationCpu";record=mean, vector);

    gates:

        input clientIN;
        output clientOUT;

        input webServerIN;
        output webServerOUT;

        input hdIN;
        output hdOUT;
}
