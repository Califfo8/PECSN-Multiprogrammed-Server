import pandas as pd
import numpy as np
import statsmodels.api as sm
import matplotlib.pyplot as plt
import scipy.stats as st
import math as m

class DataAnalysis:
    def __init__(self, factor_names, data_name):
        percorso = "Data_DA/" + data_name
        # Privati
        self._raw_data = pd.DataFrame(pd.read_excel(percorso))
        # Converto l'indice delle repliche in un vettore di interi
        indice_repliche = self._raw_data['Replication'].values
        for i in range(len(indice_repliche)):
            indice_repliche[i] = int(indice_repliche[i].replace("#", ""))
        # Trovo il numero di ripetizioni
        self._num_repliche = max(indice_repliche) + 1
        # Pubblici
        self.factor_names = factor_names
        self.num_factors = len(factor_names)
        self.num_experiments = self._raw_data.shape[0]//self._num_repliche
        self.scenario_matrix = None

    def _strfact_to_vet(self, vet, car='$'):
        vet = vet.replace(car, "")
        factors_vet = vet.rsplit(", ")
        factors = np.zeros(shape=self.num_factors)
        if self.num_factors == 0:
            print("ERRORE: Non è stato inizializzato il numero di fattori")
            return factors

        for i in range(self.num_factors):
            pos, val = factors_vet[i].rsplit("=")
            pos = int(pos)
            val = float(val)
            factors[pos] = val
        return factors

    def find_factor_values(self):

        # Creo la matrice dei fattori
        values = np.zeros(shape=(self.num_factors, 3))

        # Trovo i fattori
        for i in range(self.num_factors):
            valori = self.scenario_matrix[self.factor_names[i]]
            values[i][0] = min(valori)
            values[i][2] = max(valori)
            for v in valori:
                if v > values[i][0] and v < values[i][2]:
                    values[i][1] = v
                    break

        # Creo la tabella
        self.factors = pd.DataFrame(values, columns=["Low", 'Medium', "High"], index=self.factor_names)
        # Esporto in excel
        self.factors.to_excel("Result_DA/Factors.xlsx")

    def create_scenario_matrix(self):
        ordered_data = self._raw_data.sort_values('Measurement')
        trh = ordered_data['Value'].values
        mean_value = np.zeros(shape=(self.num_experiments, 1))
        confidence_interval = np.zeros(shape=(self.num_experiments, 2))
        confi = []
        for i in range(self.num_experiments):
            trh_scenario = trh[i * self._num_repliche: (i+1) * self._num_repliche]

            confi.append(self._strfact_to_vet(ordered_data['Measurement'][i * self._num_repliche]))
            mean_value[i] = np.mean(trh_scenario)
            ci = st.norm.interval(alpha=0.99,
                             loc=mean_value[i],
                             scale=st.sem(trh_scenario))
            confidence_interval[i][0] = -(ci[1][0]-mean_value[i])
            confidence_interval[i][1] = ci[1][0] - mean_value[i]

        # Racchiudo tutto in un dataframe
        df1 = pd.DataFrame(confi, columns=self.factor_names)
        df2 = pd.DataFrame(mean_value, columns=['Throughput'])
        df3 = pd.DataFrame(confidence_interval, columns=['neg_Delta_CI', 'Delta_CI'])
        df = pd.concat([df1, df2, df3], axis=1, join='inner')
        self.scenario_matrix = df
        self.find_factor_values()
        df.to_excel("Result_DA/scenario_analysis.xlsx")

    def plot_two_factor_graph(self, A, B, fixed_value='Medium'):
        A_name = self.factor_names[A]
        B_name = self.factor_names[B]

        # Verifico i parametri fixed
        fixed = []
        for i in range(self.num_factors):
            if i != A and i != B:
                fixed.append([i, self.factors[fixed_value][i]])

        cond_factor_one = self.scenario_matrix[self.factor_names[fixed[0][0]]] == fixed[0][1]
        cond_factor_two = self.scenario_matrix[self.factor_names[fixed[1][0]]] == fixed[1][1]
        righe_selezionate = self.scenario_matrix[cond_factor_one & cond_factor_two]
        righe_selezionate.sort_values(by=[B_name], inplace=True, ignore_index=True)
        # Creo il grafico con le rette
        # plt.clf()
        for i in range((righe_selezionate.shape[0]//3)):
            x_y = righe_selezionate[[A_name, 'Throughput']][i*3:(i+1)*3]
            valori_B = righe_selezionate[B_name]
            x_y.sort_values(by=[self.factor_names[A]], inplace=True)
            label = B_name + " = " + str(valori_B[i*3])
            plt.plot(x_y[A_name], x_y['Throughput'], label=label,)
        titolo = A_name + " vs " + B_name
        nome_file = titolo.replace(" ", "_")
        nome_file = nome_file + ".png"
        titolo = titolo + ": " + fixed_value
        plt.title(titolo)
        plt.grid()
        plt.legend(loc="upper left")
        plt.xlabel(A_name)
        plt.ylabel('Throughput')

        return nome_file
        #plt.savefig('Result_DA/Graphs/' + nome_file + '.png', dpi=300)

    def full_plot(self):
        fixed_value = ["Low", 'Medium', "High"]
        subplot_index = 1
        for i in range(self.num_factors):
            for j in range(i+1, self.num_factors):
                for k in fixed_value:
                    plt.subplot(1, 3, subplot_index)
                    plt.gcf().set_size_inches(16, 4)
                    nome_file = self.plot_two_factor_graph(i, j, k)
                    subplot_index += 1
                plt.savefig('Result_DA/Graphs/' + nome_file, dpi=300)
                plt.clf()
                subplot_index = 1

