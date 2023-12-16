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

package multiprogrammed_server.simulations;

import multiprogrammed_server.Client;
import multiprogrammed_server.Server;
import multiprogrammed_server.WebServer;
//
// TODO auto-generated type
//
network System
{
    parameters:
        @display("bgb=740,406");
    submodules:
        client: Client {
            @display("p=540,87");
        }
        server: Server {
            @display("p=316,87;is=vl");
        }
        webServer: WebServer {
            @display("p=213,249");
        }
    connections:

        webServer.out --> server.webServerIN;
        server.webServerOUT --> webServer.in;

        client.out --> server.clientIN;
        server.clientOUT --> client.in;
}
