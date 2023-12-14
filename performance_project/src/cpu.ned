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

package performance_project;

//
// TODO auto-generated type
//
simple Cpu
{
    parameters:
        double p1 = default(0.3); // shouldn't they be in the compound module? It doesn't change the behaviour of the code
        double p2 = default(0.3);
        double procTime @unit(s) = exponential(1s);
        	// non mi torna procTime, come decido quanto vale il rate (in questo caso 1s)
        	// devo mettere direttamente che il fattore è il rate e quindi poi nel codice
        	// .cc prendo il parametro da qui e ci calcolo sopra l'esponenziale?
        	// visto che poi va cambiato il rate per fare le statistiche e si fa dal .ini

			// 14/12/2023: I prefer using here just the mean value and not the exponential
		// aggiungere le statistic

    gates:
        // o così o si usa un vettore di clientIN e clientOUT
        input clientIN;
        output clientOUT;

        input webServerIN;
        output webServerOUT;

        input hdIN;
        output hdOUT;
}
